#include "mel_filter.h"

#include <stdio.h>

#include "matrix.h"

inline double hz_to_mel(double hz) {
  return 2595.0 * std::log10(1.0 + hz / 700.0);
}

inline double mel_to_hz(double mel) {
  return 700.0 * (std::pow(10.0, mel / 2595.0) - 1.0);
}

void MelFilter::CreateFilterBank() {
  int numFreq = this->fftSize / 2 + 1;
  this->filterBankData.assign(this->numFilters * numFreq, 0.0f);
  matrix_init_f32(&this->filterBank, this->numFilters, numFreq,
                  this->filterBankData.data());

  double fmin = 0;
  double fmax = this->sampleFrequency / 2;

  double mel_min = hz_to_mel(fmin);
  double mel_max = hz_to_mel(fmax);

  // Compute equally spaced mel banks
  std::vector<double> mels(this->numFilters + 2);
  for (int i = 0; i < this->numFilters + 2; ++i) {
    mels[i] = mel_min + (mel_max - mel_min) * i / (this->numFilters + 1);
  }

  // Convert mel banks into Hz. This gives logarithmically spaced frequencies.
  std::vector<double> hz(this->numFilters + 2);
  for (int i = 0; i < this->numFilters + 2; ++i) {
    hz[i] = mel_to_hz(mels[i]);
  }

  // Convert Hz to FFT bin numbers
  std::vector<double> bins(this->numFilters + 2);
  for (int i = 0; i < this->numFilters + 2; ++i) {
    bins[i] = (this->fftSize) * hz[i] / this->sampleFrequency;
  }

  for (int i = 0; i < this->numFilters; ++i) {
    double leftBin = bins[i];
    double centerBin = bins[i + 1];
    double rightBin = bins[i + 2];

    if (centerBin <= leftBin || centerBin >= rightBin) {
      continue;
    }

    // Rising slope in triangle filter banks
    int leftIdx = static_cast<int>(std::floor(leftBin));
    int centerIdx = static_cast<int>(std::ceil(centerBin));
    for (int j = leftIdx; j < centerIdx; ++j) {
      if (j >= 0 && j < numFreq) {
        this->filterBank.pData[i * numFreq + j] =
            (j - leftBin) / (centerBin - leftBin);
      }
    }

    // Falling slope in triangle filter banks
    centerIdx = static_cast<int>(std::floor(centerBin));
    int rightIdx = static_cast<int>(std::ceil(rightBin));
    for (int j = centerIdx; j < rightIdx; ++j) {
      if (j >= 0 && j < numFreq) {
        this->filterBank.pData[i * numFreq + j] =
            (rightBin - j) / (rightBin - centerBin);
      }
    }
  }
}

MelFilter::MelFilter(uint16_t numFilters, uint16_t fftSize,
                     uint16_t sampleFrequency)
    : numFilters(numFilters),
      fftSize(fftSize),
      sampleFrequency(sampleFrequency) {
  this->numFilters = numFilters;
  this->fftSize = fftSize;
  this->sampleFrequency = sampleFrequency;
  this->CreateFilterBank();
}

void MelFilter::apply(matrix& stftMatrix, matrix& melSpectrogram,
                      std::vector<float>& melSpectrogramVector) const {
  uint16_t numFrames = stftMatrix.numRows;
  const int numFreq = this->fftSize / 2 + 1;

  matrix filterBankT;
  std::vector<float> filterBankTData(numFreq * this->numFilters, 0.0f);
  matrix_init_f32(&filterBankT, numFreq, this->numFilters,
                  filterBankTData.data());
  matrix_transpose_f32(&this->filterBank, &filterBankT);

  melSpectrogramVector.assign(numFrames * this->numFilters, 0.0f);
  matrix_init_f32(&melSpectrogram, numFrames, this->numFilters,
                  melSpectrogramVector.data());

  matrix_mult_f32(&stftMatrix, &filterBankT, &melSpectrogram);
}
