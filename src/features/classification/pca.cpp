/**
 ******************************************************************************
 * @file    pca.cpp
 * @brief   Principle Component Analysis (PCA) source code.
 ******************************************************************************
 */

#include "pca.h"

#include <stdio.h>

#include "classification_constants.h"

void PrincipleComponentAnalysis::initializePCAData() {
  // Initialize the PCA projection matrix and mean vector with predefined
  // values.
  this->pcaProjection.projectionMatrix = PCA_PROJECTION_MATRIX;
  this->pcaProjection.meanVector = PCA_MEAN_VECTOR;
}

PrincipleComponentAnalysis::PrincipleComponentAnalysis(uint16_t numEigenvectors,
                                                       uint16_t numMFCCCoeffs)
    : numEigenvectors(numEigenvectors),
      numMFCCCoeffs(numMFCCCoeffs),
      pcaProjection(numEigenvectors) {
  // Initialize the PCA projection matrix and mean vector with predefined
  // values.
  this->numEigenvectors = numEigenvectors;
  this->numMFCCCoeffs = numMFCCCoeffs;
  this->initializePCAData();
}

void PrincipleComponentAnalysis::apply(
    const matrix& mfccFeatureVector, matrix& pcaFeature,
    std::vector<float>& pcaFeatureVector) const {
  const uint16_t numFrames = mfccFeatureVector.numRows;
  const uint16_t numCoeffs = mfccFeatureVector.numCols;
  if (numCoeffs != this->numMFCCCoeffs) {
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
          this->pcaProjection.meanVector[coeff];
    }
  }

  matrix projectionT;
  std::vector<float> projectionTData(numCoeffs * this->numEigenvectors, 0.0f);
  matrix_init_f32(&projectionT, numCoeffs, this->numEigenvectors,
                  projectionTData.data());
  matrix_transpose_f32(&this->pcaProjection.projectionMatrix, &projectionT);

  pcaFeatureVector.assign(numFrames * this->numEigenvectors, 0.0f);
  matrix_init_f32(&pcaFeature, numFrames, this->numEigenvectors,
                  pcaFeatureVector.data());

  matrix_mult_f32(&centeredMatrix, &projectionT, &pcaFeature);
  // printf("PCA Apply: numFrames=%u, numEigenvectors=%u\n", numFrames,
  //        this->numEigenvectors);
}
