/**
 ******************************************************************************
 * @file    pca.cpp
 * @brief   Principle Component Analysis (PCA) source code.
 ******************************************************************************
 */

#include "pca.h"

#include <stdio.h>

#include "classification_constants.h"


std::vector<float> PCA_MEAN_VECTOR;

std::vector<float> PCA_PROJECTION_MATRIX_DATA;
matrix PCA_PROJECTION_MATRIX;

void PrincipleComponentAnalysis::initializePCAData() {
  // Initialize the PCA projection matrix and mean vector with predefined
  // values.

  PCA_MEAN_VECTOR = {
    9.94755363, 2.80622458, -1.22489440, -0.15745239, -0.17760618, 0.24383263
  };

  PCA_PROJECTION_MATRIX_DATA = {
    0.99450815, -0.10281198, 0.01165613, -0.00188354, -0.01290235, -0.00879581, 0.10118255, 0.96754146, -0.19424100, -0.08988219, -0.08626934, 0.01936027, 0.01585529,
    0.22675711, 0.88002598, 0.37235996, 0.18333150, -0.04026981, 0.01733513, 0.02617091, -0.41429085, 0.71510667, 0.53809595, 0.16264352, 0.01279059, 0.02420260,
    0.09847789, -0.57627505, 0.73967010, 0.33218667, 0.00038661, -0.02457091, 0.07971204, 0.09897089, -0.34945902, 0.92796832
  };

  PCA_PROJECTION_MATRIX = {6, 6,
                                       PCA_PROJECTION_MATRIX_DATA.data()};

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
}
