/**
 ******************************************************************************
 * @file    classification_constants.cpp
 * @brief   Classification-specific constants for PCA and LDA source code.
 ******************************************************************************
 */

#include "classification.h"

#include <stdio.h>

#include "constants.h"
#include "matrix.h"

void Classification::GenerateSTFT(
    const std::vector<FrequencyDomain>& audioSignal, matrix& stftData,
    std::vector<float>& stftDataVector) const {
  const uint16_t numFrames = static_cast<uint16_t>(audioSignal.size());
  const uint16_t numFreqBins = this->n_fft / 2 + 1;

  stftDataVector.assign(numFrames * numFreqBins, 0.0f);
  matrix_init_f32(&stftData, numFrames, numFreqBins, stftDataVector.data());

  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * numFreqBins;
    for (uint16_t bin = 0; bin < numFreqBins; ++bin) {
      stftData.pData[rowStart + bin] =
          std::pow(audioSignal[frame].magnitude[bin], 2.0f);
    }
  }
}

Classification::Classification(uint16_t n_fft, uint16_t numMelFilters,
                               uint16_t numDCTCoeff, uint16_t numPCAComponents,
                               uint16_t numClasses)
    : n_fft(n_fft),
      numMelFilters(numMelFilters),
      numDCTCoeff(numDCTCoeff),
      numPCAComponents(numPCAComponents),
      numClasses(numClasses),
      fft(n_fft, SAMPLE_FREQUENCY),
      melFilter(numMelFilters, n_fft, SAMPLE_FREQUENCY),
      dct(numDCTCoeff, numMelFilters),
      pca(numPCAComponents, numDCTCoeff),
      lda(numPCAComponents, numClasses),
      currClassification(ClassificationLabel::Unknown) {}

void Classification::Classify(std::vector<float> rawAudio) {
  // Currently there is fixed logic that there will be 4 frames of fft of size
  // n_fft to run classification on. However, this is subject to change based on
  // hyperparameter tuning that will happen later on.
  if (rawAudio.size() < static_cast<size_t>(4 * this->n_fft)) {
    return;
  }

  std::vector<FrequencyDomain> fftData;
  std::vector<float> frame1(rawAudio.begin(), rawAudio.begin() + this->n_fft);
  std::vector<float> frame2(rawAudio.begin() + this->n_fft,
                            rawAudio.begin() + 2 * this->n_fft);
  std::vector<float> frame3(rawAudio.begin() + 2 * this->n_fft,
                            rawAudio.begin() + 3 * this->n_fft);
  std::vector<float> frame4(rawAudio.begin() + 3 * this->n_fft,
                            rawAudio.begin() + 4 * this->n_fft);

  FrequencyDomain fd1 =
      this->fft.signalToFrequency(frame1, WindowFunction::HANN_WINDOW);
  FrequencyDomain fd2 =
      this->fft.signalToFrequency(frame2, WindowFunction::HANN_WINDOW);
  FrequencyDomain fd3 =
      this->fft.signalToFrequency(frame3, WindowFunction::HANN_WINDOW);
  FrequencyDomain fd4 =
      this->fft.signalToFrequency(frame4, WindowFunction::HANN_WINDOW);
  fftData.push_back(fd1);
  fftData.push_back(fd2);
  fftData.push_back(fd3);
  fftData.push_back(fd4);

  matrix stftSpec;
  std::vector<float> stftDataVector;
  this->GenerateSTFT(fftData, stftSpec, stftDataVector);

  matrix melSpec;
  std::vector<float> melSpectrogramVector;
  this->melFilter.apply(stftSpec, melSpec, melSpectrogramVector);

  matrix mfccSpec;
  std::vector<float> mfccSpectrogramVector;
  this->dct.apply(melSpec, mfccSpec, mfccSpectrogramVector);

  matrix pcaSpec;
  std::vector<float> pcaFeatureVector;
  this->pca.apply(mfccSpec, pcaSpec, pcaFeatureVector);

  this->currClassification = StringToClassification(this->lda.apply(pcaSpec));
}
