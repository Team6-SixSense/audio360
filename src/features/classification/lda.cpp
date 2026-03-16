/**
 ******************************************************************************
 * @file    lda.cpp
 * @brief   Linear Discriminant Analysis (LDA) souce code.
 ******************************************************************************
 */

#include <cmath>
#include <stdio.h>

#include "lda.h"

#include "classificationLabel.h"
#include "classification_constants.h"
#include "constants.h"


std::vector<float> LDA_CLASS_WEIGHTS_DATA;

matrix LDA_CLASS_WEIGHTS;

std::vector<float> LDA_CLASS_BIASES;

std::vector<float> LDA_CLASS_BIASES_NOT_EMBEDDED;

std::vector<float> LDA_SCALINGS_DATA;

matrix LDA_SCALINGS;

std::vector<ClassificationLabel> CLASSIFICATION_CLASSES ;

void LinearDiscriminantAnalysis::initializeLDAData() {
  // Three-class model (fire, screech, truck_reversing) trained on 13 MFCC → 6 PCA features.
  LDA_CLASS_WEIGHTS_DATA = {
    0.02029330f,  0.22484505f, -0.27969456f,  0.04802127f,  0.02491694f,  0.13984840f,
    0.02037128f, -1.14728367f,  1.35300350f, -5.84238291f, -0.02378825f, -5.70755434f,
   -0.28557804f, -2.88707638f,  3.60632396f,  0.51290333f, -0.34079650f, -0.78794265f
  };

  LDA_CLASS_WEIGHTS = {3, 6, LDA_CLASS_WEIGHTS_DATA.data()};

  LDA_CLASS_BIASES = { -1.5f, 0.0f, 21.0f };
  LDA_CLASS_BIASES_NOT_EMBEDDED = {1.5f, -24.0f, -37.5f};

  LDA_SCALINGS_DATA = {
   -0.04404649f, -0.01851906f, -0.46230766f,  0.01840315f,  0.57647359f, -0.01081683f,
    0.00610506f,  0.91545147f, -0.05316798f, -0.01479128f, -0.19400401f,  0.82470751f
  };

  LDA_SCALINGS = {6, 2, LDA_SCALINGS_DATA.data()};

  CLASSIFICATION_CLASSES = {
    ClassificationLabel::Fire, ClassificationLabel::Screech, ClassificationLabel::TruckReversing};

  this->ldaProjection.classWeights = LDA_CLASS_WEIGHTS;

  // MCU build (ARM_BUILD) keeps the embedded-friendly biases; host/test builds
  // use the full-precision biases to match training.
#if ARM_BUILD
  this->ldaProjection.classBiases = LDA_CLASS_BIASES;
#else
  this->ldaProjection.classBiases = LDA_CLASS_BIASES_NOT_EMBEDDED;
#endif

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

  // Use scikit coef_ directly: classWeights (numClasses x numEigenvectors)
  const uint16_t featLen = this->ldaProjection.classWeights.numCols;
  if (featLen != this->numEigenvectors) {
    return ClassificationLabel::Unknown;
  }

  // scores = X (numFrames x featLen) * W^T (featLen x numClasses)
  std::vector<float> wTData(static_cast<size_t>(featLen) * this->numClasses, 0.0f);
  matrix wT;
  matrix_init_f32(&wT, featLen, this->numClasses, wTData.data());
  if (matrix_transpose_f32(&this->ldaProjection.classWeights, &wT) != ARM_MATH_SUCCESS) {
    return ClassificationLabel::Unknown;
  }

  std::vector<float> scoresData(static_cast<size_t>(numFrames) * this->numClasses, 0.0f);
  matrix scores;
  matrix_init_f32(&scores, numFrames, this->numClasses, scoresData.data());
  if (matrix_mult_f32(&pcaFeatureVector, &wT, &scores) != ARM_MATH_SUCCESS) {
    return ClassificationLabel::Unknown;
  }

  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * this->numClasses;
    for (uint16_t c = 0; c < this->numClasses; ++c) {
      scores.pData[rowStart + c] += this->ldaProjection.classBiases[c];
    }
  }

  float totalConfidence = 0.0f;
  std::vector<float> scoreSums(this->numClasses, 0.0f);

  // 3) Per-frame argmax + majority vote (same as before)
  std::vector<int> classCounts(this->numClasses, 0);
  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * this->numClasses;
    float maxScore = scores.pData[rowStart];
    float total = std::exp(maxScore);
    uint16_t best = 0;
    for (uint16_t c = 1; c < this->numClasses; ++c) {
      const float s = scores.pData[rowStart + c];
      total += std::exp(s);
      if (s > maxScore) {
        maxScore = s;
        best = c;
      }
    }
    totalConfidence += std::exp(maxScore)/total;
    classCounts[best]++;
    for (uint16_t c = 0; c < this->numClasses; ++c) {
      scoreSums[c] += scores.pData[rowStart + c];
    }
  }
  totalConfidence /= numFrames;
  
  if (totalConfidence < CONFIDENCE_THRESHOLD) {
    return ClassificationLabel::Unknown;
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
