/**
 ******************************************************************************
 * @file    mel_filter.cpp
 * @brief   Mel Filter Module source code (memory-optimized).
 ******************************************************************************
 */

#include "mel_filter.h"

#include <cmath>
#include <cstdint>
#include <cstdio>

#include "matrix.h"

// Use float math to reduce code size + CPU (and avoid double temporaries).
static inline float hz_to_mel_f(float hz) {
  return 2595.0f * std::log10(1.0f + hz / 700.0f);
}

static inline float mel_to_hz_f(float mel) {
  return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f);
}

MelFilter::MelFilter(uint16_t numFilters, uint16_t fftSize,
                     uint16_t sampleFrequency)
    : numFilters(numFilters),
      fftSize(fftSize),
      sampleFrequency(sampleFrequency) {
  this->CreateFilterBank();
}

void MelFilter::CreateFilterBank() {
  const int numFreq = static_cast<int>(this->fftSize / 2U + 1U);

  memset(this->filterBankTData, 0, sizeof(this->filterBankTData));

  // Store TRANSPOSED filter bank directly: (numFreq x numFilters)
  // so apply() can do: (numFrames x numFreq) * (numFreq x numFilters)

  matrix_init_f32(&this->filterBankT, static_cast<uint16_t>(numFreq),
                  this->numFilters, this->filterBankTData);

  const float fmin = 0.0f;
  const float fmax = static_cast<float>(this->sampleFrequency) * 0.5f;

  const float melMin = hz_to_mel_f(fmin);
  const float melMax = hz_to_mel_f(fmax);

  // Mel breakpoints in Hz, size = numFilters + 2

  float* melHz = new float[this->numFilters + 2U];
  memset(melHz, 0, sizeof(float) * (this->numFilters + 2U));

  for (uint16_t i = 0; i < this->numFilters + 2U; ++i) {
    const float mel = melMin + (melMax - melMin) * static_cast<float>(i) /
                                   static_cast<float>(this->numFilters + 1U);
    melHz[i] = mel_to_hz_f(mel);
  }

  // librosa-style triangular weights evaluated at true FFT bin center freqs
  for (uint16_t m = 0; m < this->numFilters; ++m) {
    const float leftHz = melHz[m];
    const float centerHz = melHz[m + 1U];
    const float rightHz = melHz[m + 2U];

    const float riseDen = centerHz - leftHz;
    const float fallDen = rightHz - centerHz;

    if (riseDen <= 0.0f || fallDen <= 0.0f) {
      continue;
    }

    for (int j = 0; j < numFreq; ++j) {
      const float fftHz =
          (static_cast<float>(j) * static_cast<float>(this->sampleFrequency)) /
          static_cast<float>(this->fftSize);

      const float lower = (fftHz - leftHz) / riseDen;
      const float upper = (rightHz - fftHz) / fallDen;

      float w = lower;
      if (upper < w) w = upper;
      if (w < 0.0f) w = 0.0f;

      // filterBankT row-major: row = bin, col = filter
      this->filterBankT.pData[static_cast<size_t>(j) * this->numFilters + m] =
          w;
    }
  }

  delete[] melHz;
}

void MelFilter::apply(matrix& stftMatrix, matrix& melSpectrogram,
                      float* melSpectrogramVector) const {
  const uint16_t numFrames = stftMatrix.numRows;
  // stftMatrix is (numFrames x numFreq)
  // filterBankT is (numFreq x numFilters)
  // result is (numFrames x numFilters)

  matrix_init_f32(&melSpectrogram, numFrames, this->numFilters,
                  melSpectrogramVector);

  // No transpose, no heap allocation here.
  matrix_mult_f32(&stftMatrix, &this->filterBankT, &melSpectrogram);
}
