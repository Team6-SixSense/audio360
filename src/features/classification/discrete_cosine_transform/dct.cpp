#include "dct.h"

#include <stdio.h>

#include <cmath>

void DiscreteCosineTransform::CreateDCTMatrix() {
  this->dctMatrix_.data.assign(this->numMelFilters_ * this->numCoefficients_,
                               0.0f);
  matrix_init_f32(&this->dctMatrix_.mat, this->numMelFilters_,
                  this->numCoefficients_, this->dctMatrix_.data.data());

  const double scale0 = 1.0f / std::sqrt(this->numMelFilters_);
  for (int i = 0; i < this->numMelFilters_; ++i) {
    this->dctMatrix_.mat.pData[i * this->numCoefficients_] =
        static_cast<float>(scale0);
  }

  const double scale = std::sqrt(2.0f / this->numMelFilters_);
  for (int i = 1; i < this->numCoefficients_; ++i) {
    for (int j = 0; j < this->numMelFilters_; ++j) {
      this->dctMatrix_.mat.pData[j * this->numCoefficients_ + i] =
          std::cos((i * M_PI * (2.0f * j + 1.0f)) /
                   (2.0f * this->numMelFilters_)) *
          scale;
    }
  }
}

DiscreteCosineTransform::DiscreteCosineTransform(uint16_t numCoefficients,
                                                 uint16_t numMelFilters)
    : numCoefficients_(numCoefficients),
      numMelFilters_(numMelFilters),
      dctMatrix_(numCoefficients) {
  this->numCoefficients_ = numCoefficients;
  this->numMelFilters_ = numMelFilters;
  this->CreateDCTMatrix();
}

DiscreteCosineTransform::~DiscreteCosineTransform() {}

void DiscreteCosineTransform::Apply(
    const matrix& melSpectrogram, matrix& mfccSpectrogram,
    std::vector<float>& mfccSpectrogramVector) const {
  const uint16_t numFrames = melSpectrogram.numRows;
  const uint16_t numMelFilters = melSpectrogram.numCols;
  if (numMelFilters != this->numMelFilters_) {
    return;
  }

  std::vector<float> logMelData(numFrames * numMelFilters, 0.0f);
  matrix logMel;
  matrix_init_f32(&logMel, numFrames, numMelFilters, logMelData.data());
  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * numMelFilters;
    for (uint16_t melBin = 0; melBin < numMelFilters; ++melBin) {
      logMel.pData[rowStart + melBin] =
          std::log(melSpectrogram.pData[rowStart + melBin] + 1e-10f);
    }
  }

  mfccSpectrogramVector.assign(numFrames * this->numCoefficients_, 0.0f);
  matrix_init_f32(&mfccSpectrogram, numFrames, this->numCoefficients_,
                  mfccSpectrogramVector.data());

  matrix_mult_f32(&logMel, &this->dctMatrix_.mat, &mfccSpectrogram);
  printf("DCT Applied: mfccSpectrogram shape: %u x %u\n",
         mfccSpectrogram.numRows, mfccSpectrogram.numCols);
}
