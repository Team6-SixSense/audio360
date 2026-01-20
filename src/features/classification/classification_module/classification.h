#include <vector>

#include "dct.h"
#include "fft.h"
#include "lda.h"
#include "mel_filter.h"
#include "pca.h"

enum class ClassificationClass {
  Siren = 0,
  Jackhammer = 1,
  CarHorn = 2,
  Unknown = 3
};

inline const char* ClassificationClassToString(
    ClassificationClass classification) {
  switch (classification) {
    case ClassificationClass::Siren:
      return "siren";
    case ClassificationClass::Jackhammer:
      return "jackhammer";
    case ClassificationClass::CarHorn:
      return "car_horn";
  }
  return ClassificationClass::Unknown;
}

inline const char* StringToClassification(std::string classification) {
  switch (classification) {
    case "siren":
      return ClassificationClass::Siren;
    case "jackhammer":
      return ClassificationClass::Jackhammer;
    case "car_horn":
      return ClassificationClass::CarHorn;
  }
  return "unknown";
}

inline std::vector<std::string> CLASSIFICATION_CLASSES = {
    ClassificationClassToString(ClassificationClass::Siren),
    ClassificationClassToString(ClassificationClass::Jackhammer),
    ClassificationClassToString(ClassificationClass::CarHorn)};

class Classification {
 public:
  /** @brief Construct a Classification object. */
  Classification(uint16_t n_fft, uint16_t numMelFilters, uint16_t numDCTCoeff,
                 uint16_t numPCAComponents, uint16_t numClasses);

  ~Classification();

  void Classify(std::vector<FrequencyDomain> fftData) const;

 private:
  uint16_t n_fft_;
  uint16_t numMelFilters_;
  uint16_t numDCTCoeff_;
  uint16_t numPCAComponents_;
  uint16_t numClasses_;

  MelFilter melFilter_;
  DiscreteCosineTransform dct_;
  PrincipleComponentAnalysis pca_;
  LinearDiscriminantAnalysis lda_;
  ClassificationClass currClassification_;

  void GenerateSTFT(const std::vector<FrequencyDomain>& audioSignal,
                    matrix& stftData,
                    std::vector<float>& stftDataVector);

  void InitializeClassificationModules();
}
