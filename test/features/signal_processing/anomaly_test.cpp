/**
 ******************************************************************************
 * @file    anomaly_test.cpp
 * @brief   Unit tests for audio anomaly detection (clipping, lost signal, silence).
 *          Tests correspond to Test-FR-3.5 in VnVPlan.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "constants.h"

/**
 * @brief Enum for anomaly types.
 */
enum class AnomalyType {
  NONE = 0,
  CLIPPING = 1,
  LOST_SIGNAL = 2,
  SILENCE = 3
};

/**
 * @brief Structure to hold anomaly detection flags.
 */
struct AnomalyFlags {
  bool clipping = false;
  bool lostSignal = false;
  bool silence = false;
};

/**
 * @brief Detects anomalies in audio signal.
 * @param signal Input audio signal.
 * @return AnomalyFlags structure with detected anomalies.
 */
static AnomalyFlags detectAnomalies(const std::vector<float>& signal) {
  AnomalyFlags flags;

  if (signal.empty()) {
    flags.silence = true;
    return flags;
  }

  const float CLIPPING_THRESHOLD = 1.0f;
  const float SILENCE_THRESHOLD = 0.001f;
  const float LOST_SIGNAL_ZERO_DURATION = 0.1f;  // 10% of signal

  // Check for clipping (amplitude exceeds 1.0)
  for (float sample : signal) {
    if (std::fabs(sample) > CLIPPING_THRESHOLD) {
      flags.clipping = true;
      break;
    }
  }

  // Check for silence (all samples near zero)
  size_t silentSamples = 0;
  for (float sample : signal) {
    if (std::fabs(sample) < SILENCE_THRESHOLD) {
      silentSamples++;
    }
  }

  if (silentSamples >= signal.size() * 0.95) {  // 95% or more silent
    flags.silence = true;
  }

  // Check for lost signal (sudden transition to zeros)
  if (!flags.silence && !flags.clipping) {
    size_t consecutiveZeros = 0;
    size_t maxConsecutiveZeros = 0;

    for (float sample : signal) {
      if (std::fabs(sample) < SILENCE_THRESHOLD) {
        consecutiveZeros++;
        if (consecutiveZeros > maxConsecutiveZeros) {
          maxConsecutiveZeros = consecutiveZeros;
        }
      } else {
        consecutiveZeros = 0;
      }
    }

    // Lost signal detected if significant portion becomes zero
    size_t lostSignalThreshold = static_cast<size_t>(signal.size() * LOST_SIGNAL_ZERO_DURATION);
    if (maxConsecutiveZeros > lostSignalThreshold) {
      flags.lostSignal = true;
    }
  }

  return flags;
}

/**
 * @brief Generates a sine wave signal.
 */
static std::vector<float> generateSineWave(float frequency, int sampleRate,
                                           float duration, float amplitude = 1.0f) {
  int numSamples = static_cast<int>(sampleRate * duration);
  std::vector<float> signal(numSamples);

  for (int i = 0; i < numSamples; i++) {
    float t = static_cast<float>(i) / sampleRate;
    signal[i] = amplitude * std::sin(TWO_PI_32 * frequency * t);
  }

  return signal;
}

/**
 * @brief Test-FR-3.5: Clipping detection.
 * @details Verifies that signals with amplitude > 1.0 are flagged as clipping.
 * @see VnVPlan.tex Section 3.1.3 (Audio Filtering Tests)
 */
TEST(AnomalyTest, ClippingDetection) {
  const int sampleRate = 16000;
  const float frequency = 1000.0f;
  const float duration = 1.0f;

  // Generate signal that exceeds 1.0 amplitude (clipping)
  std::vector<float> clipped = generateSineWave(frequency, sampleRate,
                                                 duration, 1.5f);

  AnomalyFlags flags = detectAnomalies(clipped);

  EXPECT_TRUE(flags.clipping);
  EXPECT_FALSE(flags.lostSignal);
  EXPECT_FALSE(flags.silence);
}

/**
 * @brief Test-FR-3.5: Lost signal detection.
 * @details Verifies that signals transitioning to zeros are flagged as lost signal.
 * @see VnVPlan.tex Section 3.1.3 (Audio Filtering Tests)
 */
TEST(AnomalyTest, LostSignalDetection) {
  const int sampleRate = 16000;
  const float frequency = 1000.0f;
  const float duration = 1.0f;

  // Generate signal that becomes zero halfway
  std::vector<float> signal = generateSineWave(frequency, sampleRate, duration);

  // Replace second half with zeros
  size_t halfPoint = signal.size() / 2;
  std::fill(signal.begin() + halfPoint, signal.end(), 0.0f);

  AnomalyFlags flags = detectAnomalies(signal);

  EXPECT_FALSE(flags.clipping);
  EXPECT_TRUE(flags.lostSignal);
  EXPECT_FALSE(flags.silence);
}

/**
 * @brief Test-FR-3.5: Silence detection.
 * @details Verifies that 2 seconds of zero amplitude is flagged as silence.
 * @see VnVPlan.tex Section 3.1.3 (Audio Filtering Tests)
 */
TEST(AnomalyTest, SilenceDetection) {
  const int sampleRate = 16000;
  const float duration = 2.0f;

  // Generate 2 seconds of silence
  std::vector<float> silence(static_cast<size_t>(sampleRate * duration), 0.0f);

  AnomalyFlags flags = detectAnomalies(silence);

  EXPECT_FALSE(flags.clipping);
  EXPECT_FALSE(flags.lostSignal);
  EXPECT_TRUE(flags.silence);
}

/**
 * @brief Test normal signal (no anomalies).
 */
TEST(AnomalyTest, NormalSignalNoAnomalies) {
  std::vector<float> normal = generateSineWave(1000.0f, 16000, 1.0f, 0.8f);

  AnomalyFlags flags = detectAnomalies(normal);

  EXPECT_FALSE(flags.clipping);
  EXPECT_FALSE(flags.lostSignal);
  EXPECT_FALSE(flags.silence);
}

/**
 * @brief Test very low amplitude signal (near silence but not quite).
 */
TEST(AnomalyTest, VeryLowAmplitudeNotSilence) {
  std::vector<float> lowAmp = generateSineWave(1000.0f, 16000, 1.0f, 0.01f);

  AnomalyFlags flags = detectAnomalies(lowAmp);

  // Low amplitude signal should not trigger silence flag
  EXPECT_FALSE(flags.silence);
  EXPECT_FALSE(flags.clipping);
}
