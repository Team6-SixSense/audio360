/**
 ******************************************************************************
 * @file    dct.h
 * @brief   Discrete Cosine Transform (DCT) Header
 ******************************************************************************
 */
#include <vector>

#include "matrix.h"

struct dctMatrix {
  uint16_t numCoefficients;

  std::vector<float> data;
  matrix mat;

  dctMatrix(uint16_t coeffs) : numCoefficients(coeffs), data(), mat() {}
};

class DiscreteCosineTransform {
 public:
  /** @brief Construct a DiscreteCosineTransform object. */
  DiscreteCosineTransform(uint16_t numCoefficients, uint16_t numMelFilters);

  /** @brief Destroy the DiscreteCosineTransform object. */
  ~DiscreteCosineTransform();

  /**
   * @brief Apply the DCT to the input Mel spectrogram.
   *
   * @param melSpectrogram Input Mel spectrogram, of size frames x
   * numMelFilters.
   * @param dctCoefficients Output DCT coefficients, of size frames x
   * numCoefficients.
   */
  void Apply(const matrix& melSpectrogram, matrix& mfccSpectrogram,
             std::vector<float>& mfccSpectrogramVector) const;

 private:
  uint16_t numCoefficients_;
  uint16_t numMelFilters_;
  dctMatrix dctMatrix_;

  /**
   * @brief Create the DCT transformation matrix.
   */
  void CreateDCTMatrix();
};
