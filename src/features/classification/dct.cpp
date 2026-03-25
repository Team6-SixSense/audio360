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

DiscreteCosineTransform::DiscreteCosineTransform(uint16_t numCoefficients,
                                                 uint16_t numMelFilters)
    : numCoefficients(numCoefficients),
      numMelFilters(numMelFilters),
      dctMatrixData(numCoefficients) {
  this->numCoefficients = numCoefficients;
  this->numMelFilters = numMelFilters;
  this->CreateDCTMatrix();
}

void DiscreteCosineTransform::CreateDCTMatrix() {
  matrix_init_f32(&this->dctMatrixData.mat, this->numMelFilters,
                  this->numCoefficients, this->dctMatrixData.data);

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

void DiscreteCosineTransform::apply(const matrix& melSpectrogram,
                                    matrix& mfccSpectrogram,
                                    float* mfccSpectrogramVector) {
  const uint16_t numFrames = melSpectrogram.numRows;
  const uint16_t numMelFilters = melSpectrogram.numCols;
  if (numMelFilters != this->numMelFilters) {
    return;
  }

  memset(this->logMelData, 0, sizeof(float) * numFrames * numMelFilters);

  matrix logMel;
  matrix_init_f32(&logMel, numFrames, numMelFilters, logMelData);

  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * numMelFilters;
    for (uint16_t melBin = 0; melBin < numMelFilters; ++melBin) {
      const float val =
          std::log(melSpectrogram.pData[rowStart + melBin] + 1e-10f);
      logMel.pData[rowStart + melBin] = val;
    }
  }

  matrix_init_f32(&mfccSpectrogram, numFrames, this->numCoefficients,
                  mfccSpectrogramVector);

  matrix_mult_f32(&logMel, &this->dctMatrixData.mat, &mfccSpectrogram);

  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * this->numCoefficients;
    mfccSpectrogram.pData[rowStart + 0] = 0.0f;
  }
}
