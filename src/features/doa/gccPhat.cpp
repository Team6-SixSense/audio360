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
      gccPhatFreqDomain(numSamples / 2 + 1),
      fft(numSamples, sampleFrequency),
      ifft(numSamples) {}

float GCCPhaT::calculateDirection(std::vector<float>& mic1Data,
                                  std::vector<float>& mic2Data,
                                  std::vector<float>& mic3Data,
                                  std::vector<float>& mic4Data) {
  // Compute FT for each input source.
  FrequencyDomain mic1FreqDomain = fft.signalToFrequency(mic1Data);
  FrequencyDomain mic2FreqDomain = fft.signalToFrequency(mic2Data);
  FrequencyDomain mic3FreqDomain = fft.signalToFrequency(mic3Data);
  FrequencyDomain mic4FreqDomain = fft.signalToFrequency(mic4Data);

  // Compute time delay between each microphone. Exclude mics that are diagonal
  // from each other.
  float timeDelay1_2_s = this->computeTimeDelay2Source(
      mic1FreqDomain, mic2FreqDomain, MAX_DELAY_MIC1_2);  // Horizontal.
  float timeDelay3_2_s = this->computeTimeDelay2Source(
      mic3FreqDomain, mic2FreqDomain, MAX_DELAY_MIC2_3);  // Vertical.
  float timeDelay4_3_s = this->computeTimeDelay2Source(
      mic4FreqDomain, mic3FreqDomain, MAX_DELAY_MIC3_4);  // Horizontal.
  float timeDelay4_1_s = this->computeTimeDelay2Source(
      mic4FreqDomain, mic1FreqDomain, MAX_DELAY_MIC4_1);  // Vertical.

  return this->estimateAngle(timeDelay1_2_s, timeDelay4_3_s, timeDelay3_2_s,
                             timeDelay4_1_s);
}

float GCCPhaT::computeTimeDelay2Source(const FrequencyDomain& freqA,
                                       const FrequencyDomain& freqB,
                                       float maxDelay_s) {
  this->computeGCCPhaT(freqA, freqB);
  std::vector<float> gccPhatTimeDomain =
      ifft.frequencyToTime(this->gccPhatFreqDomain);

  return this->calculateTimeDelay(gccPhatTimeDomain, maxDelay_s);
}

void GCCPhaT::computeGCCPhaT(const FrequencyDomain& freqA,
                             const FrequencyDomain& freqB) {
  for (size_t i = 0; i < this->gccPhatFreqDomain.N; i++) {
    // PhaT: removes magnitude information and keeps only phase.
    float scale = 1 / (freqA.magnitude[i] * freqB.magnitude[i]);

    // GCC.
    float real = freqA.real[i] * freqB.real[i] * scale;
    float img = freqA.img[i] * (-1.0 * freqB.img[i]) * scale;

    float magnitude = std::sqrt(real * real + img * img);

    // Store in a frequency domain struct to be processed later.
    this->gccPhatFreqDomain.real[i] = real;
    this->gccPhatFreqDomain.img[i] = img;
    this->gccPhatFreqDomain.magnitude[i] = magnitude;
    this->gccPhatFreqDomain.powerMagnitude[i] = magnitude * magnitude;
  }
}

float GCCPhaT::calculateTimeDelay(const std::vector<float>& timeDomain,
                                  float maxDelay_s) {
  float touMax = 0.0f;
  const float totMaxDelay_s = std::min(maxDelay_s + FLOAT_EPS, FLOAT_MAX);

  // Peak detection.
  for (float timeVal : timeDomain) {
    if (std::abs(timeVal) > touMax &&
        std::abs(timeVal / sampleFrequency) <= totMaxDelay_s) {
      touMax = timeVal;
    }
  }

  return touMax / sampleFrequency;
}

float GCCPhaT::estimateAngle(float timeDelayX1, float timeDelayX2,
                             float timeDelayY1, float timeDelayY2) {
  // Calculate average time delays.
  float avgTimeDelayX_s = (timeDelayX1 + timeDelayX2) / 2.0;
  float avgTimeDelayY_s = (timeDelayY1 + timeDelayY2) / 2.0;

  // Compute angle and transform to correct coordinate system (FR5.3).
  float angle = std::atan2(avgTimeDelayY_s, avgTimeDelayX_s);

  // Rotate pi/2 counterclock wise so that angle of 0 faces towards the front of
  // the glasses frame.
  angle -= PI_32 / 2.0;

  // Convert angle to be in range of 0 and 2pi.
  angle = normalizeAngleRad(angle);

  return angle;
}
