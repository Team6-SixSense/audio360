/**
 ******************************************************************************
 * @file    gccPhat.cpp
 * @brief   Generalized Cross-Correlation with Phase Transform (GCC-PhaT)
 *          source.
 ******************************************************************************
 */

#include "gccPhat.h"

#include <cmath>
#include <limits>

#include "angles.hpp"
#include "constants.h"

constexpr inline float MAX_DELAY_MIC1_2 = MIC1_2_DISTANCE_m / SOUND_AIR_mps;
constexpr inline float MAX_DELAY_MIC2_3 = MIC2_3_DISTANCE_m / SOUND_AIR_mps;
constexpr inline float MAX_DELAY_MIC3_4 = MIC3_4_DISTANCE_m / SOUND_AIR_mps;
constexpr inline float MAX_DELAY_MIC4_1 = MIC4_1_DISTANCE_m / SOUND_AIR_mps;

GCCPhaT::GCCPhaT(size_t numSamples, int sampleFrequency)
    : numSamples(numSamples),
      sampleFrequency(sampleFrequency),
      phatCrossSpectrum(numSamples / 2 + 1),
      fft(numSamples, sampleFrequency),
      ifft(numSamples) {}

float GCCPhaT::calculateDirection(std::vector<float>& mic1Data,
                                  std::vector<float>& mic2Data,
                                  std::vector<float>& mic3Data,
                                  std::vector<float>& mic4Data) {
  // Compute FT for each input source.
  FrequencyDomain mic1FreqDomain =
      fft.signalToFrequency(mic1Data, WindowFunction::HANN_WINDOW);
  FrequencyDomain mic2FreqDomain =
      fft.signalToFrequency(mic2Data, WindowFunction::HANN_WINDOW);
  FrequencyDomain mic3FreqDomain =
      fft.signalToFrequency(mic3Data, WindowFunction::HANN_WINDOW);
  FrequencyDomain mic4FreqDomain =
      fft.signalToFrequency(mic4Data, WindowFunction::HANN_WINDOW);

  // Compute time delay between each microphone. Exclude mics that are diagonal
  // from each other.
  float timeDelay1_2_s = this->estimateInterMicDelay(
      mic1FreqDomain, mic2FreqDomain, MAX_DELAY_MIC1_2);  // Horizontal.
  float timeDelay3_2_s = this->estimateInterMicDelay(
      mic3FreqDomain, mic2FreqDomain, MAX_DELAY_MIC2_3);  // Vertical.
  float timeDelay4_3_s = this->estimateInterMicDelay(
      mic4FreqDomain, mic3FreqDomain, MAX_DELAY_MIC3_4);  // Horizontal.
  float timeDelay4_1_s = this->estimateInterMicDelay(
      mic4FreqDomain, mic1FreqDomain, MAX_DELAY_MIC4_1);  // Vertical.

  return this->estimateAngle(timeDelay1_2_s, timeDelay4_3_s, timeDelay3_2_s,
                             timeDelay4_1_s);
}

float GCCPhaT::estimateInterMicDelay(const FrequencyDomain& freqA,
                                     const FrequencyDomain& freqB,
                                     float maxDelay_s) {
  this->computeGccPhatSpectrum(freqA, freqB);
  std::vector<float> gccPhatCorrelation =
      ifft.frequencyToTime(this->phatCrossSpectrum);

  return this->calculateTimeDelay(gccPhatCorrelation, maxDelay_s);
}

void GCCPhaT::computeGccPhatSpectrum(const FrequencyDomain& freqA,
                                     const FrequencyDomain& freqB) {
  for (size_t i = 0; i < this->phatCrossSpectrum.N; i++) {
    // GCC: cross correlation of frequencies.
    float real = freqA.real[i] * freqB.real[i] + freqA.img[i] * freqB.img[i];
    float img = freqA.img[i] * freqB.real[i] - freqA.real[i] * freqB.img[i];

    // PhaT: removes magnitude information and keeps only phase. This will tell
    // the timing offset of certain frequencies and remove any noise/echoes.
    float magnitude = std::sqrt(real * real + img * img);
    if (magnitude < FLOAT_EPS) {
      magnitude = FLOAT_EPS;
    }

    // Store in a frequency domain struct to be processed later.
    this->phatCrossSpectrum.real[i] = real / magnitude;
    this->phatCrossSpectrum.img[i] = img / magnitude;
    this->phatCrossSpectrum.magnitude[i] = magnitude;
    this->phatCrossSpectrum.powerMagnitude[i] = magnitude * magnitude;
  }
}

float GCCPhaT::calculateTimeDelay(const std::vector<float>& correlation,
                                  float maxDelay_s) {
  const int N = static_cast<int>(correlation.size());
  const int maxLagSamples =
      static_cast<int>(std::ceil(maxDelay_s * sampleFrequency));
  const int searchRange = std::min(maxLagSamples, N / 2 - 1);

  int bestLag = 0;
  float bestVal = -std::numeric_limits<float>::infinity();

  // Search for the strongest positive correlation peak within physical limits.
  // correlation is circularly indexed:
  // [0 .. +lags] and [N - lags .. N - 1] correspond to +/- physical delays
  for (int lag = -searchRange; lag <= searchRange; ++lag) {
    int idx = (lag >= 0) ? lag : (lag + N);
    float val = correlation[idx];

    if (val > bestVal) {
      bestVal = val;
      bestLag = lag;
    }
  }

  return static_cast<float>(bestLag) / static_cast<float>(sampleFrequency);
}

float GCCPhaT::estimateAngle(float timeDelayX1, float timeDelayX2,
                             float timeDelayY1, float timeDelayY2) {
  // Calculate average time delays on each axis.
  float avgTimeDelayX_s = (timeDelayX1 + timeDelayX2) / 2.0;
  float avgTimeDelayY_s = (timeDelayY1 + timeDelayY2) / 2.0;

  // Normalize average time by distange betweeen mics (equivalently max time
  // delay) since microphone array are not in a perfect square.
  float dx = avgTimeDelayX_s / MAX_DELAY_MIC1_2;
  float dy = avgTimeDelayY_s / MAX_DELAY_MIC2_3;

  // Compute angle and transform to correct coordinate system (FR5.3). We get
  // the below equation since:
  float angle = std::atan2(dy, dx);

  // Rotate pi/2 counterclock wise so that angle of 0 faces towards the front of
  // the glasses frame.
  angle -= PI_32 / 2.0;

  // Convert angle to be in range of 0 and 2pi.
  angle = normalizeAngleRad(angle);

  return angle;
}
