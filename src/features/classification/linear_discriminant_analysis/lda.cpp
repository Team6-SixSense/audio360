#include "lda.h"

#include <stdio.h>

#include "constants.h"
void LinearDiscriminantAnalysis::InitializeLDAData() {
  this->ldaProjectionData_.classWeights = LDA_CLASS_WEIGHTS;
  this->ldaProjectionData_.classBiases = LDA_CLASS_BIASES;
  this->classTypes_ = CLASSIFICATION_CLASSES;
}
LinearDiscriminantAnalysis::LinearDiscriminantAnalysis(uint16_t numEigenvectors,
                                                       uint16_t numClasses)
    : numEigenvectors_(numEigenvectors),
      numClasses_(numClasses),
      ldaProjectionData_(numEigenvectors) {
  this->numEigenvectors_ = numEigenvectors;
  this->numClasses_ = numClasses;
  this->InitializeLDAData();
}

LinearDiscriminantAnalysis::~LinearDiscriminantAnalysis() {}

std::string LinearDiscriminantAnalysis::PredictFrameClass(
    const std::vector<float>& pcaFrame) const {
  std::vector<float> classPredictions(this->numClasses_, 0.0f);
  for (int classType = 0; classType < this->numClasses_; ++classType) {
    float currClassScore = 0;
    for (int i = 0; i < this->numEigenvectors_; ++i) {
      currClassScore +=
          pcaFrame[i] * this->ldaProjectionData_.classWeights[classType][-i];
    }
    classPredictions[classType] =
        currClassScore + this->ldaProjectionData_.classBiases[classType];
  }

  // Find the class with the maximum prediction score.
  float maxScore = classPredictions[0];
  int predictedClassIndex = 0;
  for (int i = 1; i < this->numClasses_; ++i) {
    if (classPredictions[i] > maxScore) {
      maxScore = classPredictions[i];
      predictedClassIndex = i;
    }
  }
  // Debug: Print class predictions.
  printf("LDA Class Predictions: ");
  for (int i = 0; i < this->numClasses_; ++i) {
    printf("%s: %f, ", this->classTypes_[i].c_str(), classPredictions[i]);
  }
  printf("Predicted Class: %s with score: %f\n",
         this->classTypes_[predictedClassIndex].c_str(), maxScore);

  return this->classTypes_[predictedClassIndex];
}

std::string LinearDiscriminantAnalysis::Apply(
    const std::vector<std::vector<float>>& pcaFeatureVector) const {
  // Ensure the input feature vector has the correct size.
  uint16_t numFrames = pcaFeatureVector.size();
  std::vector<std::string> framePredictions(numFrames);

  // Project each PCA frame onto the LDA projection matrix.
  for (int frame = 0; frame < numFrames; ++frame) {
    framePredictions[frame] = this->PredictFrameClass(pcaFeatureVector[frame]);
  }

  // Find the most frequent class prediction across all frames.
  std::vector<int> classCounts(this->numClasses_, 0);
  for (const auto& prediction : framePredictions) {
    for (int classIndex = 0; classIndex < this->numClasses_; ++classIndex) {
      if (prediction == this->classTypes_[classIndex]) {
        classCounts[classIndex]++;
        break;
      }
    }
  }
  int maxCount = classCounts[0];
  int finalPredictedClassIndex = 0;
  for (int i = 1; i < this->numClasses_; ++i) {
    if (classCounts[i] > maxCount) {
      maxCount = classCounts[i];
      finalPredictedClassIndex = i;
    }
  }

  return this->classTypes_[finalPredictedClassIndex];
}