/**
 ******************************************************************************
 * @file    frequency_transform_test.cpp
 * @brief   Unit test for converting time-domain to frequency-domain (FR-3.1).
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

#include "AudioFile.h"
#include "constants.h"
#include "fft.h"

namespace {

/** @brief Finds the index of the highest magnitude bin within a tolerance of
 * the target frequency; returns -1 if no bin is within tolerance. */
int peakIndexNear(const float* freq, const float* mag, float targetHz,
                  float toleranceHz) {
  int bestIdx = -1;
  float bestMag = 0.0f;
  for (size_t i = 0; i < FREQ_DOMAIN_SIZE; ++i) {
    if (std::abs(freq[i] - targetHz) <= toleranceHz && mag[i] > bestMag) {
      bestMag = mag[i];
      bestIdx = static_cast<int>(i);
    }
  }
  return bestIdx;
}

/** @brief Computes the largest magnitude outside exclusion windows around the
 * provided peak centers. */
float maxOutsideWindows(const float* mag, const std::vector<int>& centers,
                        int halfWidth) {
  float maxVal = 0.0f;
  for (size_t i = 0; i < FREQ_DOMAIN_SIZE; ++i) {
    bool inWindow = false;
    for (int c : centers) {
      if (std::abs(static_cast<int>(i) - c) <= halfWidth) {
        inWindow = true;
        break;
      }
    }
    if (!inWindow) {
      maxVal = std::max(maxVal, mag[i]);
    }
  }
  return maxVal;
}

/** @brief Convenience wrapper to run the FFT over a frame using Hann windowing.
 */
void runFft(FrequencyDomain& out, std::vector<float> frame) {
  FFT fft(static_cast<uint16_t>(frame.size()), SAMPLE_FREQUENCY);
  fft.signalToFrequency(frame.data(), out, WindowFunction::HANN_WINDOW);
}

}  // namespace

/** @brief Given a 3-second WAV containing 100 Hz, 1 kHz, and 7.5 kHz tones,
 * the FFT output should show peaks at those frequencies and no other peaks of
 * comparable magnitude. NFR: spectrum error vs. ideal reference must be <10%
 * per bin. Note: 7.5 kHz is used instead of 8 kHz to avoid Nyquist frequency
 * issues (8 kHz is exactly at the Nyquist limit for 16 kHz sampling rate). */
TEST(AudioFilteringTest, TimeToFrequencyShowsThreePeaks) {
  AudioFile<float> audio;
  ASSERT_TRUE(audio.load("audio/three_tone.wav"));
  ASSERT_EQ(audio.getSampleRate(), SAMPLE_FREQUENCY);
  ASSERT_EQ(audio.getNumChannels(), 1);
  const auto& samples = audio.samples[0];
  ASSERT_FALSE(samples.empty());

  // Grab a clean 4096-sample window from the middle to avoid encoder padding.
  constexpr size_t frameLen = WAVEFORM_SAMPLES;  // 4096
  const size_t offset = 16000;                   // start at 1 second
  ASSERT_LE(offset + frameLen, samples.size());

  std::vector<float> frame(frameLen);
  for (size_t i = 0; i < frameLen; ++i) {
    frame[i] = samples[offset + i];
  }

  FrequencyDomain spectrum;
  runFft(spectrum, frame);

  const float binSize =
      static_cast<float>(SAMPLE_FREQUENCY) / static_cast<float>(frameLen);
  const float toleranceHz = 1.5f * binSize;  // allow small bin drift

  const float targets[] = {100.0f, 1000.0f, 7500.0f};
  std::vector<int> peakCenters;
  peakCenters.reserve(3);

  for (float target : targets) {
    int idx = peakIndexNear(spectrum.frequency, spectrum.magnitude, target,
                            toleranceHz);
    ASSERT_NE(idx, -1) << "No peak near " << target << " Hz";
    std::cout << "Peak near " << target
              << " Hz magnitude: " << spectrum.magnitude[idx] << std::endl;
    peakCenters.push_back(idx);
  }

  // Peaks should be prominent compared to non-target bins.
  const int halfWidthBins = 2;  // cover main lobe with Hann window.
  const float maxOutside =
      maxOutsideWindows(spectrum.magnitude, peakCenters, halfWidthBins);

  for (int idx : peakCenters) {
    EXPECT_GT(spectrum.magnitude[idx], 5.0f * maxOutside);
  }

  // NFR: Compare peak magnitudes against ideal analytical spectrum (<10%
  // error).
  std::vector<float> ideal(frameLen);
  for (size_t i = 0; i < frameLen; ++i) {
    const float n = static_cast<float>(offset + i);
    ideal[i] = 0.20f * std::sin(TWO_PI_32 * 100.0f * n / SAMPLE_FREQUENCY) +
               0.20f * std::sin(TWO_PI_32 * 1000.0f * n / SAMPLE_FREQUENCY) +
               0.60f * std::sin(TWO_PI_32 * 7500.0f * n / SAMPLE_FREQUENCY);
  }

  FrequencyDomain reference;
  runFft(reference, ideal);

  for (size_t t = 0; t < peakCenters.size(); ++t) {
    const int measIdx = peakCenters[t];
    // Find the nearest bin in the reference spectrum to the measured peak.
    float targetFreq = spectrum.frequency[measIdx];
    int refIdx = peakIndexNear(reference.frequency, reference.magnitude,
                               targetFreq, toleranceHz);
    ASSERT_NE(refIdx, -1);

    const float refMag = reference.magnitude[refIdx];
    const float measMag = spectrum.magnitude[measIdx];
    const float relErr = std::abs(measMag - refMag) / (refMag + 1e-9f);
    EXPECT_LT(relErr, 0.10f + 1e-5f)
        << "Peak at ~" << targetFreq << " Hz differs by >10%";
  }
}
