/**
 ******************************************************************************
 * @file    pca.h
 * @brief   Principle Component Analysis (PCA) Header
 ******************************************************************************
 */
#include <vector>

#include "constants.h"
#include "matrix.h"
#include "runtime_audio360.hpp"

struct pcaProjectionData {
  /** @brief Number of eigenvectors retained in the projection. */
  uint16_t numEigenvectors;

  /** @brief Projection matrix for PCA. */
  matrix projectionMatrix;

  /** @brief Mean vector used to center input features. */
  float meanVector[NUM_DCT_COEFF];

  pcaProjectionData(uint16_t eigenvectors)
      : numEigenvectors(eigenvectors), projectionMatrix(), meanVector() {}
};

class PrincipleComponentAnalysis {
 public:
  /** @brief Construct a PrincipleComponentAnalysis object. */
  PrincipleComponentAnalysis(uint16_t numEigenvectors, uint16_t numMFCCCoeffs);

  /**
   * @brief Project a centered frame onto the PCA projection matrix.
   *
   * @param centeredFrame Input centered frame, of size numMFCCCoeffs.
   * @param pcaFrame Output PCA frame, of size numEigenvectors.
   */
  void projectFrame(const std::vector<float>& centeredFrame,
                    std::vector<float>& pcaFrame,
                    std::vector<float>& pcaFeatureVector) const;

  /**
   * @brief Apply PCA to the input feature matrix.
   *
   * @param mfccFeatureVector Input feature matrix, of size featureLength.
   * @param pcaFeature Output PCA feature vector, of size
   * numEigenvectors.
   */
  void apply(const matrix& mfccFeatureVector, matrix& pcaFeature,
             float* pcaFeatureVector);

 private:
  /** @brief Number of PCA eigenvectors to keep. */
  uint16_t numEigenvectors;

  /** @brief Number of MFCC coefficients per frame. */
  uint16_t numMFCCCoeffs;

  /** @brief Cached PCA projection data (matrix + mean). */
  pcaProjectionData pcaProjection;

  float centeredData[CLASSIFICATION_BUFFER_SIZE * NUM_DCT_COEFF];

  void initializePCAData();
};
