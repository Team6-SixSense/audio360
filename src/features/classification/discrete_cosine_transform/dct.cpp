#include "dct.h"

#include <stdio.h>

void DiscreteCosineTransform::CreateDCTMatrix() {
  this->dctMatrix_.matrix.resize(
      this->numCoefficients_, std::vector<float>(this->numMelFilters_, 0.0f));

  const double scale0 = 1.0f / std::sqrt(this->numMelFilters_);
  for (int i = 0; i < this->numMelFilters_; ++i) {
    this->dctMatrix_.matrix[0][i] = static_cast<float>(scale0);
  }

  const double scale = std::sqrt(2.0f / this->numMelFilters_);
  for (int i = 0; i < this->numCoefficients_; ++i) {
    for (int j = 0; j < this->numMelFilters_; ++j) {
      this->dctMatrix_.matrix[i][j] = std::cos((i * M_PI * (2.0f * j + 1.0f)) /
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
    const std::vector<std::vector<float>>& melSpectrogram,
    std::vector<std::vector<float>>& dctCoefficients) const {
  uint16_t numFrames = melSpectrogram.size();
  dctCoefficients.resize(numFrames,
                         std::vector<float>(this->numCoefficients_, 0.0f));

  for (int frame = 0; frame < numFrames; ++frame) {
    for (int coeff = 0; coeff < this->numCoefficients_; ++coeff) {
      float dctValue = 0.0f;
      for (int melBin = 0; melBin < this->numMelFilters_; ++melBin) {
        dctValue += melSpectrogram[frame][melBin] *
                    this->dctMatrix_.matrix[coeff][melBin];
      }
      dctCoefficients[frame][coeff] = dctValue;
    }
  }
}