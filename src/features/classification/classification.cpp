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

std::string Classification::getClassificationLabel(){
  return ClassificationClassToString(this->currClassification);
}

void Classification::Classify(std::vector<float> rawAudio) {
  if (rawAudio.size() < static_cast<size_t>(this->n_fft)) {
    rawAudio.resize(this->n_fft, 0.0f);
  }

  const size_t numFrames = rawAudio.size() / this->n_fft;
  std::vector<FrequencyDomain> fftData;
  fftData.reserve(numFrames);
  for (size_t frame = 0; frame < numFrames; ++frame) {
    const size_t frameStart = frame * this->n_fft;
    std::vector<float> frameData(rawAudio.begin() + frameStart,
                                 rawAudio.begin() + frameStart + this->n_fft);
    fftData.push_back(
        this->fft.signalToFrequency(frameData, WindowFunction::HANN_WINDOW));
  }

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
