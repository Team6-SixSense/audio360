/**
 ******************************************************************************
 * @file    classification_constants.cpp
 * @brief   Classification-specific constants for PCA and LDA source code.
 ******************************************************************************
 */

#include "classification.h"

#include <stdio.h>

#include <algorithm>
#include <cmath>
#include <numeric>

#include "constants.h"
#include "matrix.h"

void Classification::GenerateSTFT(float* powerSpectra, matrix& stftData) {
  const uint16_t numFrames = this->powerFramesSize;

  matrix_init_f32(&stftData, numFrames, this->num_freq_bins, powerSpectra);

  // This is unnecessary now
  // for (uint16_t frame = 0; frame < numFrames; ++frame) {
  //   const size_t rowStart = static_cast<size_t>(frame) * numFreqBins;
  //   const float* src = &powerSpectra[frame * numFreqBins];
  //   for (uint16_t bin = 0; bin < numFreqBins; ++bin) {
  //     stftData.pData[rowStart + bin] = src[bin];
  //   }
  // }
}

Classification::Classification(uint16_t fftSize, uint16_t numMelFilters,
                               uint16_t numDCTCoeff, uint16_t numPCAComponents,
                               uint16_t numClasses)
    : fftSize(fftSize),
      num_freq_bins(fftSize / 2 + 1),
      numMelFilters(numMelFilters),
      numDCTCoeff(numDCTCoeff),
      numPCAComponents(numPCAComponents),
      numClasses(numClasses),
      fft(fftSize, SAMPLE_FREQUENCY),
      melFilter(numMelFilters, fftSize, SAMPLE_FREQUENCY),
      dct(numDCTCoeff, numMelFilters),
      pca(numPCAComponents, numDCTCoeff),
      lda(numPCAComponents, numClasses),
      currClassification(ClassificationLabel::Unknown) {
  memset(this->powerFrames, 0,
         sizeof(float) * (num_freq_bins * CLASSIFICATION_BUFFER_SIZE));
}

std::string Classification::getClassificationLabel() {
  return ClassificationClassToString(this->currClassification);
}

void Classification::classify(const float* rawAudio) {
  // Compute FFT and immediately extract power spectrum, discarding other
  // fields.

  float maxAbs = 0.0f;
  for (int i = 0; i < this->fftSize; i++) {
    maxAbs = std::max(maxAbs, std::fabs(rawAudio[i]));
  }

  memset(this->normalized, 0, sizeof(this->normalized));

  if (maxAbs <= 1.0f) {
    memcpy(this->normalized, rawAudio, sizeof(this->normalized));
  } else {
    constexpr float kDenom = 32767.0f;
    for (size_t i = 0; i < this->fftSize; ++i) {
      normalized[i] = rawAudio[i] / kDenom;
    }
  }

  float frameMean = 0.0f;
  for (int i = 0; i < this->fftSize; i++) {
    frameMean += normalized[i];
  }

  frameMean /= static_cast<float>(this->fftSize);

  constexpr float kTargetRms = 0.30f;
  constexpr float kMinRms = 1e-6f;

  float rms = 0.0f;

  for (int i = 0; i < this->fftSize; i++) {
    float v = normalized[i];
    v = v - frameMean;
    normalized[i] = v;

    // Compute RMS after mean removal
    rms += v * v;
  }

  rms = sqrt(rms / static_cast<float>(this->fftSize));

  float gain = 1.0f;

  if (rms > kMinRms) {
    gain = kTargetRms / rms;

    // Clamp gain so quiet frames do not blow up too much
    gain = std::clamp(gain, 0.25f, 4.0f);
  }

  constexpr float kSoftClipDrive = 1.5f;

  for (int i = 0; i < this->fftSize; i++) {
    float v = normalized[i];
    v = v * gain;
    v = tanh(kSoftClipDrive * v);
    v = std::clamp(v, -1.0f, 1.0f);
    normalized[i] = v;
  }

  this->fft.signalToFrequency(normalized, freq, WindowFunction::HANN_WINDOW);
  float* power = powerFrames[currFrameIndex];

  for (uint16_t i = 0; i < num_freq_bins; ++i) {
    power[i] = freq.powerMagnitude[i];
  }

  this->currFrameIndex =
      (this->currFrameIndex + 1) % CLASSIFICATION_BUFFER_SIZE;

  this->powerFramesSize = (this->powerFramesSize == CLASSIFICATION_BUFFER_SIZE)
                              ? CLASSIFICATION_BUFFER_SIZE
                              : this->powerFramesSize + 1;

  if (this->powerFramesSize < CLASSIFICATION_BUFFER_SIZE) {
    return;
  }

  this->GenerateSTFT(&powerFrames[0][0], stftSpec);

  this->melFilter.apply(stftSpec, melSpec, melSpectrogramVector);

  this->dct.apply(melSpec, mfccSpec, mfccSpectrogramVector);

  this->pca.apply(mfccSpec, pcaSpec, pcaFeatureVector);

  this->currClassification = this->lda.apply(pcaSpec);
}
