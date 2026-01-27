/**
 ******************************************************************************
 * @file    mel_filter.cpp
 * @brief   Mel Filter Module source code (memory-optimized).
 ******************************************************************************
 */

#include "mel_filter.h"

#include <cmath>
#include <cstdint>

#include "matrix.h"

// Use float math to reduce code size + CPU (and avoid double temporaries).
static inline float hz_to_mel_f(float hz) {
  return 2595.0f * std::log10(1.0f + hz / 700.0f);
}

static inline float mel_to_hz_f(float mel) {
  return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f);
}

void MelFilter::CreateFilterBank() {
  const int numFreq = static_cast<int>(this->fftSize / 2U + 1U);

  // Store TRANSPOSED filter bank directly: (numFreq x numFilters)
  // so apply() can do: (numFrames x numFreq) * (numFreq x numFilters)
  this->filterBankTData.assign(static_cast<size_t>(numFreq) * this->numFilters, 0.0f);
  matrix_init_f32(&this->filterBankT,
                  static_cast<uint16_t>(numFreq),
                  this->numFilters,
                  this->filterBankTData.data());

  const float fmin = 0.0f;
  const float fmax = static_cast<float>(this->sampleFrequency) * 0.5f;

  const float mel_min = hz_to_mel_f(fmin);
  const float mel_max = hz_to_mel_f(fmax);

  // We only need bins[i], bins[i+1], bins[i+2] for each filter.
  // Compute all bins in a compact float array (numFilters + 2), no doubles.
  std::vector<float> bins(static_cast<size_t>(this->numFilters) + 2U, 0.0f);
  for (uint16_t i = 0; i < this->numFilters + 2U; ++i) {
    const float mel = mel_min + (mel_max - mel_min) * static_cast<float>(i) /
                                   static_cast<float>(this->numFilters + 1U);
    const float hz = mel_to_hz_f(mel);
    bins[i] = static_cast<float>(this->fftSize) * hz / static_cast<float>(this->sampleFrequency);
  }

  // Build triangular filters, but write into TRANSPOSED layout:
  // filterBankT[bin, filter] instead of filterBank[filter, bin]
  for (uint16_t i = 0; i < this->numFilters; ++i) {
    const float leftBin   = bins[i];
    const float centerBin = bins[i + 1U];
    const float rightBin  = bins[i + 2U];

    if (centerBin <= leftBin || centerBin >= rightBin) {
      continue;
    }

    const float invRise = 1.0f / (centerBin - leftBin);
    const float invFall = 1.0f / (rightBin - centerBin);

    // Rising slope
    int leftIdx   = static_cast<int>(std::floor(leftBin));
    int centerIdx = static_cast<int>(std::ceil(centerBin));
    if (leftIdx < 0) leftIdx = 0;
    if (centerIdx > numFreq) centerIdx = numFreq;

    for (int j = leftIdx; j < centerIdx; ++j) {
      const float w = (static_cast<float>(j) - leftBin) * invRise;
      // filterBankT row-major: row = j (bin), col = i (filter)
      this->filterBankT.pData[static_cast<size_t>(j) * this->numFilters + i] = w;
    }

    // Falling slope
    centerIdx = static_cast<int>(std::floor(centerBin));
    int rightIdx = static_cast<int>(std::ceil(rightBin));
    if (centerIdx < 0) centerIdx = 0;
    if (rightIdx > numFreq) rightIdx = numFreq;

    for (int j = centerIdx; j < rightIdx; ++j) {
      const float w = (rightBin - static_cast<float>(j)) * invFall;
      this->filterBankT.pData[static_cast<size_t>(j) * this->numFilters + i] = w;
    }
  }
}

MelFilter::MelFilter(uint16_t numFilters, uint16_t fftSize, uint16_t sampleFrequency)
    : numFilters(numFilters),
      fftSize(fftSize),
      sampleFrequency(sampleFrequency) {
  this->CreateFilterBank();
}

void MelFilter::apply(matrix& stftMatrix, matrix& melSpectrogram,
                      std::vector<float>& melSpectrogramVector) const {
  const uint16_t numFrames = stftMatrix.numRows;
  // stftMatrix is (numFrames x numFreq)
  // filterBankT is (numFreq x numFilters)
  // result is (numFrames x numFilters)

  melSpectrogramVector.assign(static_cast<size_t>(numFrames) * this->numFilters, 0.0f);
  matrix_init_f32(&melSpectrogram, numFrames, this->numFilters, melSpectrogramVector.data());

  // No transpose, no heap allocation here.
  matrix_mult_f32(&stftMatrix, &this->filterBankT, &melSpectrogram);
}
