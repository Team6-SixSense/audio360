
#include <cstdint>
#include <vector>

#include "fft.h"

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
  ShortTimeFourierTransformDomain(uint16_t size)
      : numFrames(size), stft(size) {}
};

struct MelSpectrogramDomain {
  /** @brief The number of frames in the Mel spectrogram */
  uint16_t numFrames;

  /** @brief The number of Mel filters applied */
  uint16_t numFilters;

  /** @brief The tranformed Mel spectrogram object */
  std::vector<std::vector<float>> mel;

  MelSpectrogramDomain(uint16_t size)
      : numFrames(size),
        numFilters(size),
        mel(size, std::vector<float>(size)) {}
};

class MelFilter {
 public:
  /** @brief Construct a MelFilter object. */
  MelFilter(uint16_t numFilters, uint16_t fftSize, int sampleFrequency);

  /** @brief Destroy the MelFilter object. */
  ~MelFilter();

  /**
   * @brief Apply the Mel filter bank to the input power spectrum.
   *
   * @param stftPowerSpectrogram Input power spectrum, of size frames x
   * (fftSize/2 + 1) [nyquist].
   * @param melSpectrogram Output Mel filter bank energies.
   */
  void Apply(const ShortTimeFourierTransformDomain& stftPowerSpectrogram,
             std::vector<std::vector<float>>& melSpectrogram) const;

 private:
  uint16_t numFilters_;
  uint16_t fftSize_;
  int sampleFrequency_;
  std::vector<std::vector<float>> filterBank_;

  /**
   * @brief Create the Mel filter bank.
   */
  void CreateFilterBank();
};