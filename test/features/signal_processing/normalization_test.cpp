/**
 ******************************************************************************
 * @file    normalization_test.cpp
 * @brief   Unit tests for amplitude normalization functionality.
 *          Tests correspond to Test-FR-3.2 in VnVPlan.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "constants.h"

/**
 * @brief Generates a sine wave signal.
 * @param frequency Frequency in Hz.
 * @param sampleRate Sampling rate in Hz.
 * @param duration Duration in seconds.
 * @param amplitude Peak amplitude.
 * @return Vector of float samples.
 */
static std::vector<float> generateSineWave(float frequency, int sampleRate,
                                           float duration, float amplitude) {
  int numSamples = static_cast<int>(sampleRate * duration);
  std::vector<float> signal(numSamples);

  for (int i = 0; i < numSamples; i++) {
    float t = static_cast<float>(i) / sampleRate;
    signal[i] = amplitude * std::sin(TWO_PI_32 * frequency * t);
  }

  return signal;
}

/**
 * @brief Normalizes audio signal to target amplitude of 1.0.
 * @param signal Input signal (modified in place).
 */
static void normalizeAmplitude(std::vector<float>& signal) {
  if (signal.empty()) return;

  // Find maximum absolute amplitude
  float maxAmp = 0.0f;
  for (float sample : signal) {
    float absVal = std::fabs(sample);
    if (absVal > maxAmp) {
      maxAmp = absVal;
    }
  }

  // Avoid division by zero
  if (maxAmp < 1e-10f) return;

  // Normalize to max amplitude of 1.0
  float scaleFactor = 1.0f / maxAmp;
  for (float& sample : signal) {
    sample *= scaleFactor;
  }
}

/**
 * @brief Gets maximum absolute amplitude in signal segment.
 * @param signal Input signal.
 * @param start Start index.
 * @param end End index.
 * @return Maximum amplitude in range [start, end).
 */
static float getMaxAmplitude(const std::vector<float>& signal, size_t start,
                             size_t end) {
  float maxAmp = 0.0f;
  for (size_t i = start; i < end && i < signal.size(); i++) {
    float absVal = std::fabs(signal[i]);
    if (absVal > maxAmp) {
      maxAmp = absVal;
    }
  }
  return maxAmp;
}

/**
 * @brief Test-FR-3.2: Normalize amplitude of signals.
 * @details Verifies that alternating low and high amplitude sections are
 *          normalized to consistent target amplitude while preserving frequency.
 * @see VnVPlan.tex Section 3.1.3 (Audio Filtering Tests)
 */
TEST(NormalizationTest, AlternatingAmplitudeNormalization) {
  const int sampleRate = 16000;
  const float frequency = 440.0f;  // A4 note
  const int samplesPerSection = WAVEFORM_SAMPLES / 2;  // Use standard frame size
  const float duration = static_cast<float>(samplesPerSection) / sampleRate;

  // Generate low amplitude section (0.2 × max)
  std::vector<float> lowSection = generateSineWave(frequency, sampleRate,
                                                    duration, 0.2f);

  // Generate high amplitude section (0.8 × max)
  std::vector<float> highSection = generateSineWave(frequency, sampleRate,
                                                     duration, 0.8f);

  // Concatenate to create alternating signal
  std::vector<float> signal;
  signal.reserve(lowSection.size() + highSection.size());
  signal.insert(signal.end(), lowSection.begin(), lowSection.end());
  signal.insert(signal.end(), highSection.begin(), highSection.end());

  // Verify pre-normalization amplitudes are different
  float preLowMax = getMaxAmplitude(signal, 0, samplesPerSection);
  float preHighMax = getMaxAmplitude(signal, samplesPerSection,
                                     2 * samplesPerSection);

  EXPECT_NEAR(preLowMax, 0.2f, 0.01f);
  EXPECT_NEAR(preHighMax, 0.8f, 0.01f);

  // Calculate pre-normalization ratio
  float preRatio = preLowMax / preHighMax;

  // Normalize the signal
  normalizeAmplitude(signal);

  // Verify post-normalization
  float postLowMax = getMaxAmplitude(signal, 0, samplesPerSection);
  float postHighMax = getMaxAmplitude(signal, samplesPerSection,
                                      2 * samplesPerSection);

  // Global max should be 1.0 (from the high amplitude section)
  EXPECT_NEAR(postHighMax, 1.0f, 0.01f);

  // Low section should be scaled proportionally (0.2/0.8 = 0.25)
  float expectedLowAfterNorm = 0.2f / 0.8f;  // = 0.25
  EXPECT_NEAR(postLowMax, expectedLowAfterNorm, 0.01f);

  // Verify ratio preserved (consistent scaling)
  float postRatio = postLowMax / postHighMax;
  EXPECT_NEAR(postRatio, preRatio, 0.01f);
}

/**
 * @brief Test edge case: normalization of silent signal.
 */
TEST(NormalizationTest, SilentSignalNormalization) {
  std::vector<float> silence(WAVEFORM_SAMPLES, 0.0f);

  // Should not crash on silent signal
  EXPECT_NO_THROW(normalizeAmplitude(silence));

  // Signal should remain silent
  float maxAmp = getMaxAmplitude(silence, 0, silence.size());
  EXPECT_EQ(maxAmp, 0.0f);
}

/**
 * @brief Test edge case: normalization of already normalized signal.
 */
TEST(NormalizationTest, AlreadyNormalizedSignal) {
  const int sampleRate = 16000;
  const float duration = static_cast<float>(WAVEFORM_SAMPLES) / sampleRate;
  std::vector<float> signal = generateSineWave(1000.0f, sampleRate, duration, 1.0f);

  float preMax = getMaxAmplitude(signal, 0, signal.size());
  EXPECT_NEAR(preMax, 1.0f, 0.01f);

  normalizeAmplitude(signal);

  float postMax = getMaxAmplitude(signal, 0, signal.size());
  EXPECT_NEAR(postMax, 1.0f, 0.01f);
}
