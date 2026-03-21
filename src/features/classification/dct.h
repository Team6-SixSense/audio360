/**
 ******************************************************************************
 * @file    dct.h
 * @brief   Discrete Cosine Transform (DCT) Header
 ******************************************************************************
 */
#include <vector>

#include "constants.h"
#include "matrix.h"
#include "runtime_audio360.hpp"

/** @brief Struct to hold DCT matrix. */
struct dctMatrix {
  uint16_t numCoefficients;  // Number of coefficients of in the matrix.

  float data[NUM_MEL_FILTERS * NUM_DCT_COEFF];  // Data representing the matrix.

  matrix mat;  // Matrix.

  dctMatrix(uint16_t coeffs) : numCoefficients(coeffs), mat() {}
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
   * @param mfccSpectrogram Output DCT coefficients, of size frames x
   * numCoefficients.
   * @param mfccSpectrogramVector Pointer to data of MFCC spectrogram to make
   * the matrix
   */
  void apply(const matrix& melSpectrogram, matrix& mfccSpectrogram,
             float* mfccSpectrogramVector);

 private:
  /** @brief Create the DCT transformation matrix. */
  void CreateDCTMatrix();

  /** @brief Number of DCT coefficients to compute. */
  uint16_t numCoefficients;

  /** @brief Number of mel filterbank bins used as input. */
  uint16_t numMelFilters;

  /** @brief Precomputed DCT transformation matrix storage. */
  dctMatrix dctMatrixData;

  /** @brief Array to hold Log Mel data. */
  float logMelData[CLASSIFICATION_BUFFER_SIZE * NUM_MEL_FILTERS];
};
