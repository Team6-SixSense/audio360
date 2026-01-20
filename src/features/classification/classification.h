/**
 ******************************************************************************
 * @file    classification.h
 * @brief   Classification Module Header
 ******************************************************************************
 */

#include <string>
#include <vector>

#include "dct.h"
#include "fft.h"
#include "lda.h"
#include "mel_filter.h"
#include "pca.h"

enum class ClassificationClass {
  Unknown = 0,
  Siren = 1,
  Jackhammer = 2,
  CarHorn = 3,
};

/**
 * @brief Converts a classification enum value into a string label.
 *
 * @param classification Enum value to convert.
 * @return String label for the enum value.
 */
inline const char* ClassificationClassToString(
    ClassificationClass classification) {
  switch (classification) {
    case ClassificationClass::Siren:
      return "siren";
    case ClassificationClass::Jackhammer:
      return "jackhammer";
    case ClassificationClass::CarHorn:
      return "car_horn";
    case ClassificationClass::Unknown:
      return "unknown";
  }
}

/**
 * @brief Maps a string label to the matching classification enum value.
 *
 * @param classification Input string label (e.g. "siren").
 * @return Enum value corresponding to the input label.
 */
inline ClassificationClass StringToClassification(std::string classification) {
  if (classification == "siren") {
    return ClassificationClass::Siren;
  }
  if (classification == "jackhammer") {
    return ClassificationClass::Jackhammer;
  }
  if (classification == "car_horn") {
    return ClassificationClass::CarHorn;
  }
  return ClassificationClass::Unknown;
}

class Classification {
 public:
  /**
   * @brief Construct a Classification object with processing dimensions.
   *
   * @param n_fft FFT size used for frequency-domain analysis.
   * @param numMelFilters Number of mel filters to apply.
   * @param numDCTCoeff Number of DCT coefficients to retain.
   * @param numPCAComponents Number of PCA components to retain.
   * @param numClasses Number of output classes supported.
   */
  Classification(uint16_t n_fft, uint16_t numMelFilters, uint16_t numDCTCoeff,
                 uint16_t numPCAComponents, uint16_t numClasses);

  /** @brief Destroy the Classification object and release resources. */
  ~Classification();

  /**
   * @brief Runs the end-to-end classification pipeline on FFT frames.
   *
   * Applies mel filtering, DCT, PCA, and LDA in sequence to infer a class.
   *
   * @param fftData Input vector of FFT frames, of size frames x
   * (fftSize/2 + 1) [nyquist].
   */
  void Classify(std::vector<FrequencyDomain> fftData);

 private:
  /** @brief FFT size used for frequency-domain processing. */
  uint16_t n_fft;
  /** @brief Number of mel filters applied to the spectrum. */
  uint16_t numMelFilters;
  /** @brief Number of DCT coefficients retained after mel filtering. */
  uint16_t numDCTCoeff;
  /** @brief Number of PCA components retained for classification. */
  uint16_t numPCAComponents;
  /** @brief Number of supported output classes. */
  uint16_t numClasses;

  /** @brief Mel filterbank processor. */
  MelFilter melFilter;
  /** @brief DCT processor for cepstral features. */
  DiscreteCosineTransform dct;
  /** @brief PCA processor for dimensionality reduction. */
  PrincipleComponentAnalysis pca;
  /** @brief LDA processor for classification. */
  LinearDiscriminantAnalysis lda;
  /** @brief Last inferred classification result. */
  ClassificationClass currClassification;

  /**
   * @brief Builds the STFT matrix representation from FFT frames.
   *
   * @param audioSignal Input vector of FFT frames, of size frames x
   * (fftSize/2 + 1) [nyquist].
   * @param stftData Output STFT matrix, of dimensions frames x
   * (fftSize/2 + 1) [nyquist].
   * @param stftDataVector Backing storage for the matrix values.
   */
  void GenerateSTFT(const std::vector<FrequencyDomain>& audioSignal,
                    matrix& stftData, std::vector<float>& stftDataVector) const;
};
