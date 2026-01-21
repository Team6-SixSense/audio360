/**
 ******************************************************************************
 * @file    dct.cpp
 * @brief   Discrete Cosine Transform (DCT) souce code.
 ******************************************************************************
 */

#include "dct.h"

#include <stdio.h>

#include <cmath>

#include "constants.h"


void DiscreteCosineTransform::CreateDCTMatrix() {
  this->dctMatrixData.data.assign(this->numMelFilters * this->numCoefficients,
                                  0.0f);
  matrix_init_f32(&this->dctMatrixData.mat, this->numMelFilters,
                  this->numCoefficients, this->dctMatrixData.data.data());

  const float scale0 = 1.0f / std::sqrt(this->numMelFilters);
  for (int i = 0; i < this->numMelFilters; ++i) {
    this->dctMatrixData.mat.pData[i * this->numCoefficients] =
        static_cast<float>(scale0);
  }

  const float scale = std::sqrt(2.0f / this->numMelFilters);
  for (int i = 1; i < this->numCoefficients; ++i) {
    for (int j = 0; j < this->numMelFilters; ++j) {
      this->dctMatrixData.mat.pData[j * this->numCoefficients + i] =
          std::cos((i * PI_32 * (2.0f * j + 1.0f)) /
                   (2.0f * this->numMelFilters)) *
          scale;
    }
  }
}

DiscreteCosineTransform::DiscreteCosineTransform(uint16_t numCoefficients,
                                                 uint16_t numMelFilters)
    : numCoefficients(numCoefficients),
      numMelFilters(numMelFilters),
      dctMatrixData(numCoefficients) {
  this->numCoefficients = numCoefficients;
  this->numMelFilters = numMelFilters;
  this->CreateDCTMatrix();
}

void DiscreteCosineTransform::apply(
    const matrix& melSpectrogram, matrix& mfccSpectrogram,
    std::vector<float>& mfccSpectrogramVector) const {
  const uint16_t numFrames = melSpectrogram.numRows;
  const uint16_t numMelFilters = melSpectrogram.numCols;
  if (numMelFilters != this->numMelFilters) {
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

  mfccSpectrogramVector.assign(numFrames * this->numCoefficients, 0.0f);
  matrix_init_f32(&mfccSpectrogram, numFrames, this->numCoefficients,
                  mfccSpectrogramVector.data());

  matrix_mult_f32(&logMel, &this->dctMatrixData.mat, &mfccSpectrogram);
  printf("DCT Applied: mfccSpectrogram shape: %u x %u\n",
         mfccSpectrogram.numRows, mfccSpectrogram.numCols);
}
