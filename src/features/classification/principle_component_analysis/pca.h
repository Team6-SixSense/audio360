#include <vector>

#include "matrix.h"

struct pcaProjectionData {
  uint16_t numEigenvectors;

  matrix matrix;
  std::vector<float> meanVector;

  pcaProjectionData(uint16_t eigenvectors)
      : numEigenvectors(eigenvectors), matrix(), meanVector() {}
};

class PrincipleComponentAnalysis {
 public:
  /** @brief Construct a PrincipleComponentAnalysis object. */
  PrincipleComponentAnalysis(uint16_t numEigenvectors, uint16_t numMFCCCoeffs);

  /** @brief Destroy the PrincipleComponentAnalysis object. */
  ~PrincipleComponentAnalysis();

  /**
   * @brief Project a centered frame onto the PCA projection matrix.
   *
   * @param centeredFrame Input centered frame, of size numMFCCCoeffs.
   * @param pcaFrame Output PCA frame, of size numEigenvectors.
   */
  void ProjectFrame(const std::vector<float>& centeredFrame,
                    std::vector<float>& pcaFrame) const;

  /**
   * @brief Apply PCA to the input feature vector.
   *
   * @param inputFeatureVector Input feature vector, of size featureLength.
   * @param pcaFeatureVector Output PCA feature vector, of size
   * numEigenvectors.
   */
  void Apply(const std::vector<std::vector<float>>& mfccFeatureVector,
             std::vector<std::vector<float>>& pcaFeatureVector) const;

  void Apply(const matrix& mfccFeatureVector,
             matrix& pcaFeatureVector,
             std::vector<float>& pcaFeatureVectorData) const;

 private:
  uint16_t numEigenvectors_;
  uint16_t numMFCCCoeffs_;
  pcaProjectionData pcaProjectionData_;

  void InitializePCAData();
};
