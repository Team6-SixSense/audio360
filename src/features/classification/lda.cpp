/**
 ******************************************************************************
 * @file    lda.cpp
 * @brief   Linear Discriminant Analysis (LDA) souce code.
 ******************************************************************************
 */

#include "lda.h"

#include <stdio.h>

#include <climits>
#include <cmath>

#include "classificationLabel.h"
#include "classification_constants.h"
#include "constants.h"

std::vector<float> LDA_CLASS_WEIGHTS_DATA;

matrix LDA_CLASS_WEIGHTS;

std::vector<float> LDA_CLASS_BIASES;

std::vector<float> LDA_CLASS_BIASES_ENV;

std::vector<float> LDA_CLASS_BIASES_NOT_EMBEDDED;

std::vector<float> LDA_SCALINGS_DATA;

matrix LDA_SCALINGS;

std::vector<ClassificationLabel> CLASSIFICATION_CLASSES;

void LinearDiscriminantAnalysis::initializeLDAData() {
  // Three-class model (fire, engine, truck_reversing) trained on 13 MFCC →
  // PCA features.
  LDA_CLASS_WEIGHTS_DATA = {
      0.19358437f,  1.38794649f,  0.19732946f,  0.19398162f,  -1.01740980f,
      0.28223020f,  -0.12278274f, -2.84530330f, 0.28168017f,  -0.02063890f,
      1.65350235f,  -0.30270889f, -1.21890199f, -0.79680419f, -2.88690639f,
      -1.63528287f, 2.33100653f,  -1.27706242f};

  LDA_CLASS_WEIGHTS = {3, 6, LDA_CLASS_WEIGHTS_DATA.data()};

  LDA_CLASS_BIASES = {-6.45036364f, 4.38645935f, -8.29020691f};

  // Use this configuration when in a room filled with people talking.
  LDA_CLASS_BIASES_ENV = {-3.05036364f, 1.38645935f, -10.29020691f};
  LDA_CLASS_BIASES_NOT_EMBEDDED = {-6.45036364f, 11.38645935f, -8.29020691f};

  LDA_SCALINGS_DATA = {-0.06232077f, -0.17988630f, -0.54079854f, 0.10227066f,
                       -0.05047226f, -0.33339098f, -0.06615186f, -0.10715678f,
                       0.42568585f,  0.19358982f,  0.16270618f,  0.17846420f};

  LDA_SCALINGS = {6, 2, LDA_SCALINGS_DATA.data()};

  CLASSIFICATION_CLASSES = {ClassificationLabel::SomeoneTalking,
                            ClassificationLabel::Siren,
                            ClassificationLabel::SmokeAlarm};

  this->ldaProjection.classWeights = LDA_CLASS_WEIGHTS;

#ifndef BUILD_TESTS
  this->ldaProjection.classBiases = LDA_CLASS_BIASES;
#else
  this->ldaProjection.classBiases = LDA_CLASS_BIASES_NOT_EMBEDDED;
#endif

  this->classTypes = CLASSIFICATION_CLASSES;
  this->ldaProjection.scalings = LDA_SCALINGS;
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

ClassificationLabel LinearDiscriminantAnalysis::apply(
    const matrix& pcaFeatureVector) const {
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
  std::vector<float> wTData(static_cast<size_t>(featLen) * this->numClasses,
                            0.0f);
  matrix wT;
  matrix_init_f32(&wT, featLen, this->numClasses, wTData.data());
  if (matrix_transpose_f32(&this->ldaProjection.classWeights, &wT) !=
      ARM_MATH_SUCCESS) {
    return ClassificationLabel::Unknown;
  }

  std::vector<float> scoresData(
      static_cast<size_t>(numFrames) * this->numClasses, 0.0f);
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
    totalConfidence += std::exp(maxScore) / total;
    classCounts[best]++;
    for (uint16_t c = 0; c < this->numClasses; ++c) {
      scoreSums[c] += scores.pData[rowStart + c];
    }
  }
  totalConfidence /= numFrames;

  if (totalConfidence < CONFIDENCE_THRESHOLD) {
    return ClassificationLabel::Unknown;
  }

  int bestClass = 0;
  int bestAverage = -1 * INT_MAX;

  // Debug: //print average per-class scores and mean confidence.
  // printf("LDA avg scores:");
  for (uint16_t c = 0; c < this->numClasses; ++c) {
    const float avgScore = scoreSums[c] / static_cast<float>(numFrames);
    if (avgScore > bestAverage) {
      bestAverage = avgScore;
      bestClass = c;
    }
    // printf(" [%s]=%.3f", ClassificationClassToString(this->classTypes[c]),
    //        avgScore);
  }
  // printf(" | avg confidence=%.3f\n", totalConfidence);

  return this->classTypes[bestClass];
}
