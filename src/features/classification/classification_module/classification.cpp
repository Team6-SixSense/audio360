#include "classification.h"

#include <stdio.h>

#include "constants.h"
#include "matrix.h"

void Classification::InitializeClassificationModules() {
  this->melFilter_ =
      MelFilter(this->numMelFilters_, this->n_fft_, SAMPLE_FREQUENCY);
  this->dct_ =
      DiscreteCosineTransform(this->numDCTCoeff_, this->numMelFilters_);
  this->pca_ =
      PrincipleComponentAnalysis(this->numPCAComponents_, this->numDCTCoeff_);
  this->lda_ =
      LinearDiscriminantAnalysis(this->numPCAComponents_, this->numClasses_);
}

void Classification::GenerateSTFT(
    const std::vector<FrequencyDomain>& audioSignal,
    matrix& stftData,
    std::vector<float>& stftDataVector) {
  const uint16_t numFrames = static_cast<uint16_t>(audioSignal.size());
  const uint16_t numFreqBins = this->n_fft_ / 2 + 1;

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
    : n_fft_(n_fft),
      numMelFilters_(numMelFilters),
      numDCTCoeff_(numDCTCoeff) numPCAComponents_(numPCAComponents),
      numClasses_(numClasses) {
  this->n_fft_ = n_fft;
  this->numMelFilters_ = numMelFilters;
  this->numDCTCoeff_ = numDCTCoeff;
  this->numPCAComponents_ = numPCAComponents;
  this->numClasses_ = numClasses;
  this->currClassification_ = ClassificationClass::Unknown;
  this->InitializeClassificationModules();
}

void Classification::Classify(std::vector<FrequencyDomain> fftData) const {
  matrix stftSpec;
  std::vector<float> stftDataVector;
  GenerateSTFT(fftData, stftSpec, stftDataVector);

  matrix melSpec;
  std::vector<float> melSpectrogramVector;
  this->melFilter_.Apply(stftSpec, melSpec, melSpectrogramVector);

  matrix mfccSpec;
  std::vector<float> mfccSpectrogramVector;
  this->dct_.Apply(melSpec, mfccSpec, mfccSpectrogramVector);

  matrix pcaSpec;
  std::vector<float> pcaFeatureVector;
  this->pca_.Apply(mfccSpec, pcaSpec, pcaFeatureVector);

  this->currClassification_ = StringToClassification(this->lda_.Apply(pcaSpec));
}
