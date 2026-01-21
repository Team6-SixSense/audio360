/**
 ******************************************************************************
 * @file    classification.h
 * @brief   Classification Module Header
 ******************************************************************************
 */

#include <string>
#include <vector>

#include "classificationLabel.h"
#include "dct.h"
#include "fft.h"
#include "lda.h"
#include "mel_filter.h"
#include "pca.h"

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

  /**
   * @brief Runs the end-to-end classification pipeline on FFT frames.
   *
   * Applies mel filtering, DCT, PCA, and LDA in sequence to infer a class.
   *
   * @param fftData Input vector of FFT frames, of size frames x
   * (fftSize/2 + 1) [nyquist].
   */
  void Classify(std::vector<float> rawAudio);

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

  /** @brief FFT module. */
  FFT fft;

  /** @brief Mel filterbank processor. */
  MelFilter melFilter;

  /** @brief DCT processor for cepstral features. */
  DiscreteCosineTransform dct;

  /** @brief PCA processor for dimensionality reduction. */
  PrincipleComponentAnalysis pca;

  /** @brief LDA processor for classification. */
  LinearDiscriminantAnalysis lda;

  /** @brief Last inferred classification result. */
  ClassificationLabel currClassification;

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
