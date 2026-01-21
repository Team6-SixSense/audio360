/**
 ******************************************************************************
 * @file    lda.h
 * @brief   Linear Discriminant Analysis (LDA) Header
 ******************************************************************************
 */

#include <string>
#include <vector>

#include "matrix.h"

struct ldaProjectionData {
  /** @brief Number of LDA components retained. */
  uint16_t numComponents;

  /** @brief LDA class weight matrix. */
  matrix classWeights;
  /** @brief Per-class bias terms for LDA. */
  std::vector<float> classBiases;

  ldaProjectionData(uint16_t components)
      : numComponents(components), classWeights(), classBiases() {}
};

class LinearDiscriminantAnalysis {
 public:
  /** @brief Construct a LinearDiscriminantAnalysis object. */
  LinearDiscriminantAnalysis(uint16_t numEigenvectors, uint16_t numClasses);

  /**
   * @brief Project a PCA frame onto the LDA projection matrix.
   *
   * @param pcaFrame Input PCA frame, of size inputFeatureLength.
   * @param ldaFrame Output LDA frame, of size numComponents.
   */
  // TODO: Update this to return enum class type.
  std::string predictFrameClass(const matrix& pcaFeatureVector,
                                uint16_t frameIndex) const;

  /**
   * @brief Apply LDA to the input feature vector.
   *
   * @param inputFeatureVector Input feature vector, of size featureLength.
   * @param ldaFeatureVector Output LDA feature vector, of size numComponents.
   */
  std::string apply(const matrix& pcaFeatureVector) const;

 private:
  /** @brief Number of PCA eigenvectors expected as input. */
  uint16_t numEigenvectors;

  /** @brief Number of classes supported by the model. */
  uint16_t numClasses;

  /** @brief Class label strings aligned with LDA outputs. */
  std::vector<std::string> classTypes;

  /** @brief Cached projection weights and biases for LDA. */
  ldaProjectionData ldaProjection;

  void initializeLDAData();
};
