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
  11.54753685f, 1.82549763f, -2.13036108f, -0.37158024f, -0.20174536f, 0.79973066f, 0.92288339f, 0.71663338f, -0.20936063f, 0.35371673f, 0.37495318f, 0.34454781f, -0.07723899f
};

  // Trained projection matrix, shape 13 x 6 (row-major: MFCC rows, PCA cols).
  PCA_PROJECTION_MATRIX_DATA = {
    0.99090934f, 0.01767388f, -0.10582951f, 0.02210451f, 0.01208457f, -0.03006382f, 0.05715685f, 0.51102531f, 0.75041682f, 0.25198314f, 0.28614575f, -0.12232912f, -0.04333366f,
    0.72056824f, -0.29547074f, -0.10817923f, -0.19348809f, 0.25119984f, -0.06195750f, 0.38112861f, -0.51560676f, -0.01502693f, 0.39739802f, -0.32680863f, 0.02301303f, 0.17547135f,
    -0.04331942f, 0.34218839f, -0.37632036f, 0.21078698f, -0.02743137f, -0.02122942f, -0.17271715f, 0.57905757f, 0.21457586f, 0.19157234f, -0.03806422f, -0.18701498f, -0.16822773f,
    0.39502940f, 0.50524282f, 0.15405108f, 0.07819749f, -0.04679121f, 0.09091463f, -0.22801109f, 0.26971671f, 0.51978332f, 0.00043767f, 0.03077066f, 0.02862481f, -0.45057636f,
    0.44264653f, -0.01711682f, 0.01523796f, 0.02137402f, 0.01504408f, -0.10508790f, 0.04639412f, -0.27462971f, 0.00187420f, -0.04175267f, -0.01403793f, 0.16722718f, -0.09101937f,
    -0.51661992f, 0.00554246f, -0.02790647f, -0.02995810f, 0.03569444f, -0.02341803f, -0.30089799f, -0.00167017f, 0.04162354f, 0.04590615f, -0.14264639f, 0.00363896f, 0.07770912f};

  PCA_PROJECTION_MATRIX = {13, 6, PCA_PROJECTION_MATRIX_DATA.data()};

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

  pcaFeatureVector.assign(numFrames * this->numEigenvectors, 0.0f);
  matrix_init_f32(&pcaFeature, numFrames, this->numEigenvectors,
                  pcaFeatureVector.data());

  // Multiply centered MFCCs (numFrames x numCoeffs) by projection (numCoeffs x
  // numEigenvectors)
  if (matrix_mult_f32(&centeredMatrix, &this->pcaProjection.projectionMatrix,
                      &pcaFeature) != ARM_MATH_SUCCESS) {
    return;
  }
}
