#include "lda.h"

#include <stdio.h>

#include "classification_constants.h"

void LinearDiscriminantAnalysis::initializeLDAData() {
  this->ldaProjectionData.classWeights = LDA_CLASS_WEIGHTS;
  this->ldaProjectionData.classBiases = LDA_CLASS_BIASES;
  this->classTypes = CLASSIFICATION_CLASSES;
}
LinearDiscriminantAnalysis::LinearDiscriminantAnalysis(uint16_t numEigenvectors,
                                                       uint16_t numClasses)
    : numEigenvectors(numEigenvectors),
      numClasses(numClasses),
      ldaProjectionData(numEigenvectors) {
  this->numEigenvectors = numEigenvectors;
  this->numClasses = numClasses;
  this->initializeLDAData();
}


std::string LinearDiscriminantAnalysis::predictFrameClass(
    const matrix& pcaFeatureVector, uint16_t frameIndex) const {
  if (pcaFeatureVector.numCols != this->numEigenvectors ||
      frameIndex >= pcaFeatureVector.numRows) {
    return {};
  }

  const size_t frameStart =
      static_cast<size_t>(frameIndex) * pcaFeatureVector.numCols;
  std::vector<float> classPredictions(this->numClasses, 0.0f);
  for (int classType = 0; classType < this->numClasses; ++classType) {
    float currClassScore = 0;
    const size_t weightStart =
        static_cast<size_t>(classType) * this->numEigenvectors;
    for (int i = 0; i < this->numEigenvectors; ++i) {
      currClassScore +=
          pcaFeatureVector.pData[frameStart + i] *
          this->ldaProjectionData.classWeights.pData[weightStart + i];
    }
    classPredictions[classType] =
        currClassScore + this->ldaProjectionData.classBiases[classType];
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

std::string LinearDiscriminantAnalysis::apply(
    const matrix& pcaFeatureVector) const {
  uint16_t numFrames = pcaFeatureVector.numRows;
  if (numFrames == 0 || pcaFeatureVector.numCols != this->numEigenvectors) {
    return {};
  }
  std::vector<float> classWeightsTData(this->numEigenvectors * this->numClasses,
                                       0.0f);
  matrix classWeightsT;
  matrix_init_f32(&classWeightsT, this->numEigenvectors, this->numClasses,
                  classWeightsTData.data());
  if (matrix_transpose_f32(&this->ldaProjectionData.classWeights,
                           &classWeightsT) != ARM_MATH_SUCCESS) {
    return {};
  }

  std::vector<float> scoresData(numFrames * this->numClasses, 0.0f);
  matrix scores;
  matrix_init_f32(&scores, numFrames, this->numClasses, scoresData.data());
  if (matrix_mult_f32(&pcaFeatureVector, &classWeightsT, &scores) !=
      ARM_MATH_SUCCESS) {
    return {};
  }

  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * this->numClasses;
    for (uint16_t classType = 0; classType < this->numClasses; ++classType) {
      scores.pData[rowStart + classType] +=
          this->ldaProjectionData.classBiases[classType];
    }
  }

  std::vector<std::string> framePredictions(numFrames);
  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * this->numClasses;
    float maxScore = scores.pData[rowStart];
    uint16_t predictedClassIndex = 0;
    for (uint16_t classType = 1; classType < this->numClasses; ++classType) {
      const float score = scores.pData[rowStart + classType];
      if (score > maxScore) {
        maxScore = score;
        predictedClassIndex = classType;
      }
    }
    framePredictions[frame] = this->classTypes[predictedClassIndex];
    printf("LDA Frame %u: ", frame);
    for (uint16_t classType = 0; classType < this->numClasses; ++classType) {
      printf("%s: %f, ", this->classTypes[classType].c_str(),
             scores.pData[rowStart + classType]);
    }
    printf("Predicted Class: %s\n",
           this->classTypes[predictedClassIndex].c_str());
  }

  // Find the most frequent class prediction across all frames.
  std::vector<int> classCounts(this->numClasses, 0);
  for (const auto& prediction : framePredictions) {
    for (int classIndex = 0; classIndex < this->numClasses; ++classIndex) {
      if (prediction == this->classTypes[classIndex]) {
        classCounts[classIndex]++;
        break;
      }
    }
  }
  int maxCount = classCounts[0];
  int finalPredictedClassIndex = 0;
  for (int i = 1; i < this->numClasses; ++i) {
    if (classCounts[i] > maxCount) {
      maxCount = classCounts[i];
      finalPredictedClassIndex = i;
    }
  }

  printf("LDA Final Prediction: %s\n",
         this->classTypes[finalPredictedClassIndex].c_str());
  return this->classTypes[finalPredictedClassIndex];
}
