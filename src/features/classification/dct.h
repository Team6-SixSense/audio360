/**
 ******************************************************************************
 * @file    dct.h
 * @brief   Discrete Cosine Transform (DCT) Header
 ******************************************************************************
 */
#include <vector>

#include "matrix.h"

/** @brief Struct to hold DCT matrix. */
struct dctMatrix {
  uint16_t numCoefficients;  // Number of coefficients of in the matrix.

  std::vector<float> data;  // Data representing the matrix.

  matrix mat;  // Matrix.

  dctMatrix(uint16_t coeffs) : numCoefficients(coeffs), data(), mat() {}
};

class DiscreteCosineTransform {
 public:
  /** @brief Construct a DiscreteCosineTransform object. */
  DiscreteCosineTransform(uint16_t numCoefficients, uint16_t numMelFilters);

  /**
   * @brief Apply the DCT to the input Mel spectrogram.
   *
   * @param melSpectrogram Input Mel spectrogram, of size frames x
   * numMelFilters.
   * @param dctCoefficients Output DCT coefficients, of size frames x
   * numCoefficients.
   */
  void apply(const matrix& melSpectrogram, matrix& mfccSpectrogram,
             std::vector<float>& mfccSpectrogramVector) const;

 private:
  /** @brief Number of DCT coefficients to compute. */
  uint16_t numCoefficients;

  /** @brief Number of mel filterbank bins used as input. */
  uint16_t numMelFilters;

  /** @brief Precomputed DCT transformation matrix storage. */
  dctMatrix dctMatrixData;

  /** @brief Create the DCT transformation matrix. */
  void CreateDCTMatrix();
};
