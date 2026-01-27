/**
 ******************************************************************************
 * @file    mel_filter.h
 * @brief   Mel Filter Module Header
 ******************************************************************************
 */

#include <cstdint>
#include <vector>

#include "fft.h"
#include "matrix.h"

struct ShortTimeFourierTransformDomain {
  /** @brief The number of frames stacked to create the stft */
  uint16_t numFrames;

  /** @brief The Short-Time Fourier Transform (STFT) object */
  std::vector<FrequencyDomain> stft;

  /**
   * @brief Construct a new STFT Domain struct.
   *
   * @param size The number of frames being stored.
   */
  ShortTimeFourierTransformDomain(uint16_t size) : numFrames(size), stft() {}
};

class MelFilter {
 public:
  /** @brief Construct a MelFilter object. */
  MelFilter(uint16_t numFilters, uint16_t fftSize, uint16_t sampleFrequency);

  /**
   * @brief Apply the Mel filter bank to the input power spectrum.
   *
   * @param stftPowerSpectrogram Input power spectrum, of size frames x
   * (fftSize/2 + 1) [nyquist].
   * @param melSpectrogram Output Mel filter bank energies.
   */
  void apply(matrix& stftPowerSpectrogram, matrix& melSpectrogram,
             std::vector<float>& melSpectrogramVector) const;

 private:
  /** @brief Number of mel filters in the bank. */
  uint16_t numFilters;

  /** @brief FFT size used to derive frequency bins. */
  uint16_t fftSize;

  /** @brief Sample rate of the input signal (Hz). */
  uint16_t sampleFrequency;

  matrix filterBankT;
  std::vector<float> filterBankTData;

  /** @brief Create the Mel filter bank. */
  void CreateFilterBank();
};
