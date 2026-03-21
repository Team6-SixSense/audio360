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

  /**
   * @brief Construct a new lda Projection Data object.
   *
   * @param components The number of components in LDA.
   */
  ldaProjectionData(uint16_t components)
      : numComponents(components), classWeights(), classBiases() {}
};

class LinearDiscriminantAnalysis {
 public:
  /**
   * @brief Construct a new Linear Discriminant Analysis object
   *
   * @param numEigenvectors The number of eigen vectors.
   * @param numClasses The number of classes.
   */
  LinearDiscriminantAnalysis(uint16_t numEigenvectors, uint16_t numClasses);

  /**
   * @brief Project a PCA frame onto the LDA projection matrix.
   *
   * @param pcaFeatureVector Matrix of PCA feature vector.
   * @param frameIndex The current frame index.
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
  /** @brief Initializes LDA data. */
  void initializeLDAData();

  /** @brief Number of PCA eigenvectors expected as input. */
  uint16_t numEigenvectors;

  /** @brief Number of classes supported by the model. */
  uint16_t numClasses;

  /** @brief Cached projection weights and biases for LDA. */
  ldaProjectionData ldaProjection;

  /** @brief WT data. */
  float wTData[NUM_PCA_COMPONENTS * NUM_CLASSES];

  /** @brief Array that holds the confidence scores for each classes. */
  float scoresData[CLASSIFICATION_BUFFER_SIZE * NUM_CLASSES];

  /** @brief Sum of scores. */
  float scoreSums[NUM_CLASSES];

  /** @brief Count of each class. */
  int classCounts[NUM_CLASSES];

  /** @brief Class prediction array. */
  float classPredictions[NUM_CLASSES];

  /** @brief LDS class weights array. */
  static float LDA_CLASS_WEIGHTS_DATA[NUM_CLASSES * NUM_PCA_COMPONENTS];

  /** @brief Matrix that holds the LDA weights. */
  matrix LDA_CLASS_WEIGHTS;

  /** @brief LDA class bias array. */
  static float LDA_CLASS_BIASES[NUM_CLASSES];

  /** @brief LDA class bias array for when not running on embedded controller.
   */
  static float LDA_CLASS_BIASES_NOT_EMBEDDED[NUM_CLASSES];

  /** @brief LDA scaling data array. */
  static float LDA_SCALINGS_DATA[NUM_PCA_COMPONENTS * (NUM_CLASSES - 1)];

  /** @brief LDA class biases in environment. */
  static float LDA_CLASS_BIASES_ENV[NUM_CLASSES];

  /** @brief Matrix that holds the LDA scalings. */
  matrix LDA_SCALINGS;

  /** @brief Mapping for class to classification label in form of an array. */
  static ClassificationLabel CLASSIFICATION_CLASSES[NUM_CLASSES];
};
