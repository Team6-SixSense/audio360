
#include "pca.h"

#include <stdio.h>

#include "constants.h"

void PrincipleComponentAnalysis::InitializePCAData() {
  // Initialize the PCA projection matrix and mean vector with predefined
  // values.
  this->pcaProjectionData_.matrix = PCA_PROJECTION_MATRIX;
  this->pcaProjectionData_.meanVector = PCA_MEAN_VECTOR;
}

PrincipleComponentAnalysis::PrincipleComponentAnalysis(uint16_t numEigenvectors,
                                                       uint16_t numMFCCCoeffs)
    : numEigenvectors_(numEigenvectors),
      numMFCCCoeffs_(numMFCCCoeffs),
      pcaProjectionData_(numEigenvectors) {
  // Initialize the PCA projection matrix and mean vector with predefined
  // values.
  this->numEigenvectors_ = numEigenvectors;
  this->numMFCCCoeffs_ = numMFCCCoeffs;
  this->InitializePCAData();
}

PrincipleComponentAnalysis::~PrincipleComponentAnalysis() {}

void PrincipleComponentAnalysis::ProjectFrame(
    const std::vector<float>& centeredFrame,
    std::vector<float>& pcaFrame) const {
  for (int i = 0; i < this->numEigenvectors_; ++i) {
    float projectionValue = 0.0f;
    for (int j = 0; j < this->numMFCCCoeffs_; ++j) {
      projectionValue +=
          centeredFrame[j] * this->pcaProjectionData_.matrix[i][j];
    }
    pcaFrame[i] = projectionValue;
  }
}

void PrincipleComponentAnalysis::Apply(
    const std::vector<std::vector<float>>& mfccFeatureVector,
    std::vector<std::vector<float>>& pcaFeatureVector) const {
  // Ensure the input feature vector has the correct size.

  uint16_t numFrames = mfccFeatureVector.size();

  // Resize the output PCA feature vector.
  pcaFeatureVector.resize(numFrames,
                          std::vector<float>(this->numEigenvectors_, 0.0f));

  // Center the input feature vector by subtracting the mean vector.
  std::vector<std::vector<float>> centeredVector(

      numFrames, std::vector<float>(this->numMFCCCoeffs_, 0.0f));

  for (int frame = 0; frame < numFrames; ++frame) {
    for (int coeff = 0; coeff < this->numMFCCCoeffs_; ++coeff) {
      centeredVector[frame][coeff] = mfccFeatureVector[frame][coeff] -
                                     this->pcaProjectionData_.meanVector[coeff];
    }
  }

  // Project the centered vector onto the PCA projection matrix.
  for (int frame = 0; frame < numFrames; ++frame) {
    this->ProjectFrame(centeredVector[frame], pcaFeatureVector[frame]);
  }
}
