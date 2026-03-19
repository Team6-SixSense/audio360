/**
 ******************************************************************************
 * @file    lda.h
 * @brief   Linear Discriminant Analysis (LDA) Header
 ******************************************************************************
 */

#include <string>
#include <vector>

#include "classificationLabel.h"
#include "matrix.h"
#include "runtime_audio360.hpp"

struct ldaProjectionData {
  /** @brief Number of LDA components retained. */
  uint16_t numComponents;

  /** @brief LDA class weight matrix. */
  matrix classWeights;
  /** @brief Per-class bias terms for LDA. */
  float classBiases[NUM_CLASSES];

  /** @brief Scalings used after data projected into the LDA space */
  matrix scalings;

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
  ClassificationLabel predictFrameClass(const matrix& pcaFeatureVector,
                                        uint16_t frameIndex);

  /**
   * @brief Apply LDA to the input feature vector.
   *
   * @param inputFeatureVector Input feature vector, of size featureLength.
   * @param ldaFeatureVector Output LDA feature vector, of size numComponents.
   */
  ClassificationLabel apply(const matrix& pcaFeatureVector);

 private:
  /** @brief Number of PCA eigenvectors expected as input. */
  uint16_t numEigenvectors;

  /** @brief Number of classes supported by the model. */
  uint16_t numClasses;

  /** @brief Cached projection weights and biases for LDA. */
  ldaProjectionData ldaProjection;

  float wTData[NUM_PCA_COMPONENTS * NUM_CLASSES];

  float scoresData[CLASSIFICATION_BUFFER_SIZE * NUM_CLASSES];

  float scoreSums[NUM_CLASSES];
  int classCounts[NUM_CLASSES];

  float classPredictions[NUM_CLASSES];

  static float LDA_CLASS_WEIGHTS_DATA[NUM_CLASSES * NUM_PCA_COMPONENTS];

  matrix LDA_CLASS_WEIGHTS;

  static float LDA_CLASS_BIASES[NUM_CLASSES];

  static float LDA_CLASS_BIASES_NOT_EMBEDDED[NUM_CLASSES];

  static float LDA_SCALINGS_DATA[NUM_PCA_COMPONENTS * (NUM_CLASSES - 1)];

  matrix LDA_SCALINGS;

  static ClassificationLabel CLASSIFICATION_CLASSES[NUM_CLASSES];

  void initializeLDAData();
};
