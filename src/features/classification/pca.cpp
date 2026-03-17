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

  PCA_MEAN_VECTOR = {78.87020111f, 0.96546859f, 1.14741635f, 1.18352711f,
                     1.30074537f,  0.77457994f, 0.48022380f, 0.48995197f,
                     -0.19950433f, 0.22589383f, 0.12943101f, 0.16440322f,
                     -0.03524146f};

  // Trained projection matrix, shape 13 x 6 (row-major: MFCC rows, PCA cols).
  PCA_PROJECTION_MATRIX_DATA = {
      0.99984294f,  0.00413575f,  -0.00577235f, 0.01338913f,  0.00773062f,
      0.00026768f,  -0.00178416f, 0.59952414f,  0.78886217f,  -0.04141825f,
      0.06196103f,  0.03274837f,  -0.01383586f, 0.62282145f,  -0.44959435f,
      0.57143986f,  0.08927422f,  -0.24886836f, -0.00378490f, 0.12200177f,
      -0.15984371f, -0.20027220f, 0.82776213f,  0.35635522f,  -0.00207745f,
      0.11430492f,  -0.10985713f, 0.19903487f,  -0.32549241f, 0.88262415f,
      -0.00103180f, -0.17859137f, 0.22654490f,  0.52707452f,  -0.05701476f,
      0.01690649f,  -0.00691815f, -0.23346825f, 0.21512637f,  0.42279568f,
      0.25480935f,  0.12023666f,  -0.00217417f, 0.04412086f,  -0.05709456f,
      -0.04797074f, 0.15176116f,  0.05161553f,  0.00605248f,  0.29842404f,
      -0.12214240f, -0.30000588f, -0.22904037f, 0.01003938f,  0.00193716f,
      0.08723557f,  -0.04551582f, -0.07135159f, -0.13880824f, 0.10833571f,
      -0.00158751f, -0.16064778f, 0.12993552f,  0.15833195f,  0.10514665f,
      -0.02218750f, -0.00039242f, -0.09522235f, 0.05326317f,  0.08678892f,
      0.08096430f,  -0.01783043f, 0.00202873f,  0.07257779f,  -0.01836078f,
      -0.07264332f, -0.12824792f, 0.03038189f};

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
