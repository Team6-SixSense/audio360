#include <vector>

struct ldaProjectionData {
  uint16_t numComponents;

  std::vector<std::vector<float>> classWeights;
  std::vector<float> classBiases;

  ldaProjectionData(uint16_t components)
      : numComponents(components), classWeights(), classBiases() {}
};

class LinearDiscriminantAnalysis {
 public:
  /** @brief Construct a LinearDiscriminantAnalysis object. */
  LinearDiscriminantAnalysis(uint16_t numEigenvectors, uint16_t numClasses);

  /** @brief Destroy the LinearDiscriminantAnalysis object. */
  ~LinearDiscriminantAnalysis();

  /**
   * @brief Project a PCA frame onto the LDA projection matrix.
   *
   * @param pcaFrame Input PCA frame, of size inputFeatureLength.
   * @param ldaFrame Output LDA frame, of size numComponents.
   */
  // TODO: Update this to return enum class type.
  std::string PredictFrameClass(const std::vector<float>& pcaFrame) const;

  /**
   * @brief Apply LDA to the input feature vector.
   *
   * @param inputFeatureVector Input feature vector, of size featureLength.
   * @param ldaFeatureVector Output LDA feature vector, of size numComponents.
   */
  std::string Apply(
      const std::vector<std::vector<float>>& pcaFeatureVector) const;

 private:
  uint16_t numEigenvectors_;
  uint16_t numClasses_;
  std::vector<std::string> classTypes_;
  ldaProjectionData ldaProjectionData_;

  void InitializeLDAData();
};