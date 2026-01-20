
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

// void PrincipleComponentAnalysis::ProjectFrame(
//     const std::vector<float>& centeredFrame,
//     std::vector<float>& pcaFrame) const {
//   for (int i = 0; i < this->numEigenvectors_; ++i) {
//     float projectionValue = 0.0f;
//     const size_t rowStart = static_cast<size_t>(i) * this->numMFCCCoeffs_;
//     for (int j = 0; j < this->numMFCCCoeffs_; ++j) {
//       projectionValue += centeredFrame[j] *
//                          this->pcaProjectionData_.matrix.pData[rowStart + j];
//     }
//     pcaFrame[i] = projectionValue;
//   }
// }

void PrincipleComponentAnalysis::Apply(
    const matrix& mfccFeatureVector, matrix& pcaFeature,
    std::vector<float>& pcaFeatureVector) const {
  const uint16_t numFrames = mfccFeatureVector.numRows;
  const uint16_t numCoeffs = mfccFeatureVector.numCols;
  if (numCoeffs != this->numMFCCCoeffs_) {
    return;
  }

  std::vector<float> centeredData(numFrames * numCoeffs, 0.0f);
  matrix centeredMatrix;
  matrix_init_f32(&centeredMatrix, numFrames, numCoeffs, centeredData.data());
  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * numCoeffs;
    for (uint16_t coeff = 0; coeff < numCoeffs; ++coeff) {
      centeredMatrix.pData[rowStart + coeff] =
          mfccFeatureVector.pData[rowStart + coeff] -
          this->pcaProjectionData_.meanVector[coeff];
    }
  }

  matrix projectionT;
  std::vector<float> projectionTData(numCoeffs * this->numEigenvectors_, 0.0f);
  matrix_init_f32(&projectionT, numCoeffs, this->numEigenvectors_,
                  projectionTData.data());
  matrix_transpose_f32(&this->pcaProjectionData_.matrix, &projectionT);

  pcaFeatureVector.assign(numFrames * this->numEigenvectors_, 0.0f);
  matrix_init_f32(&pcaFeature, numFrames, this->numEigenvectors_,
                  pcaFeatureVector.data());

  matrix_mult_f32(&centeredMatrix, &projectionT, &pcaFeature);
  printf("PCA Apply: numFrames=%u, numEigenvectors=%u\n", numFrames,
         this->numEigenvectors_);
}
