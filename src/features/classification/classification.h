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
#include "frequencyDomain.h"
#include "lda.h"
#include "mel_filter.h"
#include "pca.h"
#include "runtime_audio360.hpp"

class Classification {
 public:
  /**
   * @brief Construct a Classification object with processing dimensions.
   *
   * @param fftSize FFT size used for frequency-domain analysis.
   * @param numMelFilters Number of mel filters to apply.
   * @param numDCTCoeff Number of DCT coefficients to retain.
   * @param numPCAComponents Number of PCA components to retain.
   * @param numClasses Number of output classes supported.
   */
  Classification(uint16_t fftSize, uint16_t numMelFilters, uint16_t numDCTCoeff,
                 uint16_t numPCAComponents, uint16_t numClasses);

  /**
   * @brief Runs the end-to-end classification pipeline on FFT frames.
   *
   * Applies mel filtering, DCT, PCA, and LDA in sequence to infer a class.
   *
   * @param rawAudio Input array of FFT frames, of size frames x
   * (fftSize/2 + 1) [nyquist].
   */
  void classify(const float* rawAudio);

  /**
   * @brief Returns the classification label state value from the classification
   * module
   */
  std::string getClassificationLabel();

 private:
  /**
   * @brief Builds the STFT matrix representation from FFT frames.
   *
   * @param powerSpectra Input vector of power spectra, of size frames x
   * (fftSize/2 + 1) [nyquist].
   * @param stftData Output STFT matrix, of dimensions frames x
   * (fftSize/2 + 1) [nyquist].
   */
  void GenerateSTFT(float* powerSpectra, matrix& stftData);

  /** @brief FFT size used for frequency-domain processing. */
  uint16_t fftSize;

  uint16_t num_freq_bins;

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

  /** @brief Sliding buffer of power spectra per frame (size: n_fft/2 + 1). */
  float powerFrames[CLASSIFICATION_BUFFER_SIZE][(DOA_SAMPLES / 2) + 1];

  /** @brief Index tracking for current frame. */
  uint8_t currFrameIndex = 0;

  /** @brief The size of the power frame */
  uint8_t powerFramesSize = 0;

  /** @brief Matrix representation of the fftData vector */
  matrix stftSpec;

  /** @brief Matrix that will store the computed mel spectrogram */
  matrix melSpec;

  /** @brief Helper of melSpec, contains the actual data for the matrix */
  float melSpectrogramVector[CLASSIFICATION_BUFFER_SIZE * NUM_MEL_FILTERS];

  /** @brief Matrix that will store the computed mfcc coefficients for each
   * frame */
  matrix mfccSpec;

  /** @brief Helper of mfccSpec, contains the actual data for the matrix */
  float mfccSpectrogramVector[CLASSIFICATION_BUFFER_SIZE * NUM_DCT_COEFF];

  /** @brief Matrix that will store the data projected onto pca space */
  matrix pcaSpec;

  /** @brief Helper of pcaSpec, contains the actual data for the matrix */
  float pcaFeatureVector[CLASSIFICATION_BUFFER_SIZE * NUM_PCA_COMPONENTS];

  /** @brief Frequency domain struct. This is where FFT results be stored. */
  FrequencyDomain freq;

  // Normalization array
  float normalized[FFT_BUFFER_SIZE_IN];
};
