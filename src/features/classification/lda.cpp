/**
 ******************************************************************************
 * @file    lda.cpp
 * @brief   Linear Discriminant Analysis (LDA) souce code.
 ******************************************************************************
 */

#include "lda.h"

#include <stdio.h>

#include "classificationLabel.h"
#include "classification_constants.h"


std::vector<float> LDA_CLASS_WEIGHTS_DATA;

matrix LDA_CLASS_WEIGHTS;

std::vector<float> LDA_CLASS_BIASES;

std::vector<float> LDA_SCALINGS_DATA;

matrix LDA_SCALINGS;

std::vector<ClassificationLabel> CLASSIFICATION_CLASSES ;

void LinearDiscriminantAnalysis::initializeLDAData() {
  LDA_CLASS_WEIGHTS_DATA = {0.03074151,  1.2104454, -1.5900505,
  -0.41783714, 1.4128469 , -0.44179523};

  LDA_CLASS_WEIGHTS = {3, 2, LDA_CLASS_WEIGHTS_DATA.data()};

  LDA_CLASS_BIASES = {
    3.528, -9.737268, 7.043996
  };

  LDA_SCALINGS_DATA = {
    -0.07936350, -0.01101666,
     0.38999072, -0.04274665,
    -0.55441391,  0.00280420,
     0.21863136,  0.12364489,
    -0.07492033, -1.26101398,
     0.04050463,  0.37705261
  };

  LDA_SCALINGS = {6, 2, LDA_SCALINGS_DATA.data()};

  CLASSIFICATION_CLASSES = {
    ClassificationLabel::Jackhammer, ClassificationLabel::CarHorn, ClassificationLabel::Siren};

  this->ldaProjection.classWeights = LDA_CLASS_WEIGHTS;
  this->ldaProjection.classBiases = LDA_CLASS_BIASES;
  this->classTypes = CLASSIFICATION_CLASSES;
  this->ldaProjection.scalings     = LDA_SCALINGS;
}
LinearDiscriminantAnalysis::LinearDiscriminantAnalysis(uint16_t numEigenvectors,
                                                       uint16_t numClasses)
    : numEigenvectors(numEigenvectors),
      numClasses(numClasses),
      ldaProjection(numEigenvectors) {
  this->numEigenvectors = numEigenvectors;
  this->numClasses = numClasses;
  this->initializeLDAData();
}

ClassificationLabel LinearDiscriminantAnalysis::predictFrameClass(
    const matrix& pcaFeatureVector, uint16_t frameIndex) const {
  if (pcaFeatureVector.numCols != this->numEigenvectors ||
      frameIndex >= pcaFeatureVector.numRows) {
    return ClassificationLabel::Unknown;
  }

  const size_t frameStart =
      static_cast<size_t>(frameIndex) * pcaFeatureVector.numCols;
  std::vector<float> classPredictions(this->numClasses, 0.0f);
  for (int classType = 0; classType < this->numClasses; ++classType) {
    float currClassScore = 0;
    const size_t weightStart =
        static_cast<size_t>(classType) * this->numEigenvectors;
    for (int i = 0; i < this->numEigenvectors; ++i) {
      currClassScore += pcaFeatureVector.pData[frameStart + i] *
                        this->ldaProjection.classWeights.pData[weightStart + i];
    }
    classPredictions[classType] =
        currClassScore + this->ldaProjection.classBiases[classType];
  }

  // Find the class with the maximum prediction score.
  float maxScore = classPredictions[0];
  int predictedClassIndex = 0;
  for (int i = 1; i < this->numClasses; ++i) {
    if (classPredictions[i] > maxScore) {
      maxScore = classPredictions[i];
      predictedClassIndex = i;
    }
  }

  return this->classTypes[predictedClassIndex];
}

ClassificationLabel LinearDiscriminantAnalysis::apply(const matrix& pcaFeatureVector) const {
  const uint16_t numFrames = pcaFeatureVector.numRows;
  if (numFrames == 0 || pcaFeatureVector.numCols != this->numEigenvectors) {
    return ClassificationLabel::Unknown;
  }

  const uint16_t ldaDims = static_cast<uint16_t>(this->numClasses - 1U);

  // 1) Project PCA -> LDA space: Z = X * scalings  => (numFrames x ldaDims)
  std::vector<float> zData(static_cast<size_t>(numFrames) * ldaDims, 0.0f);
  matrix z;
  matrix_init_f32(&z, numFrames, ldaDims, zData.data());

  // ldaProjection.scalings must be (numEigenvectors x ldaDims)
  if (this->ldaProjection.scalings.numRows != this->numEigenvectors ||
      this->ldaProjection.scalings.numCols != ldaDims) {
    return ClassificationLabel::Unknown;
  }

  if (matrix_mult_f32(&pcaFeatureVector, &this->ldaProjection.scalings, &z) != ARM_MATH_SUCCESS) {
    return ClassificationLabel::Unknown;
  }

  // 2) Scores in LDA space: scores = Z * W^T + b
  // classWeights: (numClasses x ldaDims) so transpose to (ldaDims x numClasses)
  std::vector<float> wTData(static_cast<size_t>(ldaDims) * this->numClasses, 0.0f);
  matrix wT;
  matrix_init_f32(&wT, ldaDims, this->numClasses, wTData.data());

  if (matrix_transpose_f32(&this->ldaProjection.classWeights, &wT) != ARM_MATH_SUCCESS) {
    return ClassificationLabel::Unknown;
  }

  std::vector<float> scoresData(static_cast<size_t>(numFrames) * this->numClasses, 0.0f);
  matrix scores;
  matrix_init_f32(&scores, numFrames, this->numClasses, scoresData.data());

  if (matrix_mult_f32(&z, &wT, &scores) != ARM_MATH_SUCCESS) {
    return ClassificationLabel::Unknown;
  }

  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * this->numClasses;
    for (uint16_t c = 0; c < this->numClasses; ++c) {
      scores.pData[rowStart + c] += this->ldaProjection.classBiases[c];
    }
  }

  // 3) Per-frame argmax + majority vote (same as before)
  std::vector<int> classCounts(this->numClasses, 0);
  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * this->numClasses;
    float maxScore = scores.pData[rowStart];
    uint16_t best = 0;
    for (uint16_t c = 1; c < this->numClasses; ++c) {
      const float s = scores.pData[rowStart + c];
      if (s > maxScore) {
        maxScore = s;
        best = c;
      }
    }
    classCounts[best]++;
  }

  int bestCount = classCounts[0];
  int bestClass = 0;
  for (int c = 1; c < this->numClasses; ++c) {
    if (classCounts[c] > bestCount) {
      bestCount = classCounts[c];
      bestClass = c;
    }
  }
  return this->classTypes[bestClass];
}

