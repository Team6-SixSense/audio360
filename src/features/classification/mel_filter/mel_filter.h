
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

// TODO: Determine if this struct is needed

// struct MelSpectrogramDomain {
//   /** @brief The number of frames in the Mel spectrogram */
//   uint16_t numFrames;

//   /** @brief The number of Mel filters applied */
//   uint16_t numFilters;

//   /** @brief The tranformed Mel spectrogram object */
//   std::vector<std::vector<float>> mel;

//   MelSpectrogramDomain(uint16_t size)
//       : numFrames(size),
//         numFilters(size),
//         mel(size, std::vector<float>(size)) {}
// };

class MelFilter {
 public:
  /** @brief Construct a MelFilter object. */
  MelFilter(uint16_t numFilters, uint16_t fftSize, uint16_t sampleFrequency);

  /** @brief Destroy the MelFilter object. */
  ~MelFilter();

  /**
   * @brief Apply the Mel filter bank to the input power spectrum.
   *
   * @param stftPowerSpectrogram Input power spectrum, of size frames x
   * (fftSize/2 + 1) [nyquist].
   * @param melSpectrogram Output Mel filter bank energies.
   */
  void Apply(ShortTimeFourierTransformDomain& stftPowerSpectrogram,
             matrix& melSpectrogram,
             std::vector<float>& melSpectrogramData) const;

 private: 
  uint16_t numFilters_;
  uint16_t fftSize_;
  uint16_t sampleFrequency_;
  std::vector<float> filterBankData_;
  matrix filterBank_;

  /**
   * @brief Create the Mel filter bank.
   */
  void CreateFilterBank();
};
