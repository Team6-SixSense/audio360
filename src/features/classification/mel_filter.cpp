#include "features/classification/mel_filter.h"

#include <stdio.h>

inline double hz_to_mel(double hz) {
  return 2595.0 * std::log10(1.0 + hz / 700.0);
}

inline double mel_to_hz(double mel) {
  return 700.0 * (std::pow(10.0, mel / 2595.0) - 1.0);
}

void MelFilter::CreateFilterBank() {
  int numFreq = this->fftSize_ / 2 + 1;
  this->filterBank_.resize(this->numFilters_,
                           std::vector<float>(numFreq, 0.0f));

  double fmin = 0;
  int fmax = this->sampleFrequency_ / 2;

  double mel_min = hz_to_mel(fmin);
  double mel_max = hz_to_mel(fmax);

  // Compute equally spaced mel banks
  std::vector<double> mels(this->numFilters_ + 2);
  for (int i = 0; i < this->numFilters_ + 2; ++i) {
    mels[i] = mel_min + (mel_max - mel_min) * i / (this->numFilters_ + 1);
  }

  // Convert mel banks into Hz. This gives logarithmically spaced frequencies.
  std::vector<double> hz(this->numFilters_ + 2);
  for (int i = 0; i < this->numFilters_ + 2; ++i) {
    hz[i] = mel_to_hz(mels[i]);
  }

  // Convert Hz to FFT bin numbers
  std::vector<double> bins(this->numFilters_ + 2);
  for (int i = 0; i < this->numFilters_ + 2; ++i) {
    bins[i] = (this->fftSize_) * hz[i] / this->sampleFrequency_;
  }

  for (int i = 0; i < this->numFilters_; ++i) {
    int leftBin = bins[i];
    int centerBin = bins[i + 1];
    int rightBin = bins[i + 2];

    if (centerBin <= leftBin || centerBin >= rightBin) {
      continue;
    }

    // Rising slope in triangle filter banks
    int leftIdx = static_cast<int>(std::floor(leftBin));
    int centerIdx = static_cast<int>(std::ceil(centerBin));
    for (int j = leftIdx; j < centerIdx; ++j) {
      if (j >= 0 && j < numFreq) {
        this->filterBank_[i][j] = (j - leftBin) / (centerBin - leftBin);
      }
    }

    // Falling slope in triangle filter banks
    centerIdx = static_cast<int>(std::floor(centerBin));
    int rightIdx = static_cast<int>(std::ceil(rightBin));
    for (int j = centerIdx; j < rightIdx; ++j) {
      if (j >= 0 && j < numFreq) {
        this->filterBank_[i][j] = (rightBin - j) / (rightBin - centerBin);
      }
    }
  }
}

MelFilter::MelFilter(uint16_t numFilters, uint16_t fftSize,
                     uint16_t sampleFrequency)
    : numFilters_(numFilters),
      fftSize_(fftSize),
      sampleFrequency_(sampleFrequency) {
  this->numFilters_ = numFilters;
  this->fftSize_ = fftSize;
  this->sampleFrequency_ = sampleFrequency;
  this->CreateFilterBank();
}

MelFilter::~MelFilter() {}

void MelFilter::Apply(ShortTimeFourierTransformDomain& stftPowerSpectrogram,
                      std::vector<std::vector<float>>& melSpectrogram) const {
  uint16_t numFrames = stftPowerSpectrogram.numFrames;
  melSpectrogram.resize(numFrames, std::vector<float>(this->numFilters_, 0.0f));

  for (int frame = 0; frame < numFrames; ++frame) {
    for (int melBin = 0; melBin < this->numFilters_; ++melBin) {
      float melEnergy = 0.0f;
      for (int freqBin = 0; freqBin < (this->fftSize_ / 2 + 1); ++freqBin) {
        melEnergy += stftPowerSpectrogram.stft[frame].magnitude[freqBin] *
                     this->filterBank_[melBin][freqBin];
      }
      melSpectrogram[frame][melBin] = melEnergy;
    }
  }
}