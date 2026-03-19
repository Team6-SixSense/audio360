/**
 ******************************************************************************
 * @file    spectral_leakage_test.cpp
 * @brief   Unit test for reduced spectral leakage (Test-FR-3.3).
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

#include "constants.h"
#include "fft.h"

namespace {

// Half-width (in bins) of the main lobe for supported windows. Rectangular
// (no window) has first null at +/-1 bin, Hann at +/-2 bins.
/** @brief Returns the half-width (in bins) of the main lobe for the given
 * window function. */
constexpr int mainLobeHalfWidth(WindowFunction window) {
  switch (window) {
    case WindowFunction::HANN_WINDOW:
      return 2;
    case WindowFunction::NONE:
    default:
      return 1;
  }
}

/** @brief Returns the index of the maximum element in a vector. */
int findPeakIndex(const float* values) {
  int max_index = 0;

  for (int i = 0; i < FREQ_DOMAIN_SIZE; i++) {
    if (values[i] > values[max_index]) {
      max_index = i;
    }
  }

  return max_index;
}

/** @brief Calculates the proportion of total spectral power that leaks outside
 * the main lobe defined by halfWidth around the peak. */
double computeLeakage(const FrequencyDomain& spectrum, int halfWidth) {
  const auto& power = spectrum.powerMagnitude;
  const int peakIdx = findPeakIndex(power);

  const int start = std::max(0, peakIdx - halfWidth);
  const int end =
      std::min(static_cast<int>(spectrum.N - 1), peakIdx + halfWidth);

  double totalPower = 0.0;

  for (int i = 0; i < FREQ_DOMAIN_SIZE; ++i) {
    totalPower += power[i];
  }

  double mainLobePower = 0.0;
  for (int k = start; k <= end; ++k) {
    mainLobePower += power[k];
  }

  return 1.0 - (mainLobePower / totalPower);
}

/** @brief Returns the maximum side-lobe power outside the main lobe defined by
 * halfWidth around the spectral peak. */
double computeMaxSideLobe(const FrequencyDomain& spectrum, int halfWidth) {
  const auto& power = spectrum.powerMagnitude;
  const int peakIdx = findPeakIndex(power);

  double maxSide = 0.0;
  for (size_t k = 0; k < FREQ_DOMAIN_SIZE; ++k) {
    if (std::abs(static_cast<int>(k) - peakIdx) <= halfWidth) {
      continue;  // Skip main lobe bins.
    }
    if (power[k] > maxSide) {
      maxSide = power[k];
    }
  }

  return maxSide;
}

}  // namespace

/** @brief Given a truncated sine wave, windowing should reduce spectral
 * leakage by shrinking side-lobes and concentrating energy in the main lobe. */
TEST(AudioFilteringTest, ReducedSpectralLeakageWithHannWindow) {
  // Use a single analysis frame (4096 samples) cut from a 1 s recording. The
  // FFT implementation only supports power-of-two lengths up to 4096, which
  // matches the waveform buffer size used elsewhere in the pipeline.
  constexpr int sampleRate = SAMPLE_FREQUENCY;    // 16 kHz
  constexpr int signalLength = WAVEFORM_SAMPLES;  // 4096 samples (~0.256 s)

  // Pick a frequency that lands halfway between FFT bins to maximize leakage
  // in the rectangular (no window) case.
  const float binSizeHz =
      static_cast<float>(sampleRate) / static_cast<float>(signalLength);
  const float desiredBin = 200.5f;  // Half-bin offset.
  const float testFrequencyHz = desiredBin * binSizeHz;

  std::vector<float> baseSignal(signalLength);
  for (int n = 0; n < signalLength; ++n) {
    baseSignal[n] =
        std::sin(TWO_PI_32 * testFrequencyHz * static_cast<float>(n) /
                 static_cast<float>(sampleRate));
  }

  // Duplicate signals so windowing does not mutate the shared buffer.
  std::vector<float> unwindowedSignal = baseSignal;
  std::vector<float> hannSignal = baseSignal;

  FFT fft(static_cast<uint16_t>(signalLength), sampleRate);

  FrequencyDomain spectrumRect;

  fft.signalToFrequency(unwindowedSignal.data(), spectrumRect,
                        WindowFunction::NONE);
  FrequencyDomain spectrumHann;

  fft.signalToFrequency(hannSignal.data(), spectrumHann,
                        WindowFunction::HANN_WINDOW);

  const int rectHalfWidth = mainLobeHalfWidth(WindowFunction::NONE);
  const int hannHalfWidth = mainLobeHalfWidth(WindowFunction::HANN_WINDOW);

  const double leakageRect = computeLeakage(spectrumRect, rectHalfWidth);
  const double leakageHann = computeLeakage(spectrumHann, hannHalfWidth);

  const double maxSideRect = computeMaxSideLobe(spectrumRect, rectHalfWidth);
  const double maxSideHann = computeMaxSideLobe(spectrumHann, hannHalfWidth);

  // Peaks should remain centered on the injected tone in both cases.
  const int peakRect = findPeakIndex(spectrumRect.powerMagnitude);
  const int peakHann = findPeakIndex(spectrumHann.powerMagnitude);
  EXPECT_NEAR(testFrequencyHz, peakRect * binSizeHz, binSizeHz);
  EXPECT_NEAR(testFrequencyHz, peakHann * binSizeHz, binSizeHz);

  // Hann window should concentrate more energy in the main lobe and suppress
  // side-lobes relative to no windowing.
  EXPECT_LT(leakageHann, leakageRect);
  EXPECT_LT(maxSideHann, maxSideRect);
}
