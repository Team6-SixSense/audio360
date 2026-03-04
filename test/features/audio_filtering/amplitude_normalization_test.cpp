/**
 ******************************************************************************
 * @file    amplitude_normalization_test.cpp
 * @brief   Unit test for amplitude normalization (Test-FR-3.2).
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "mp3.h"

namespace {

double maxAbs(const std::vector<double>& data, size_t start, size_t end) {
  double maxVal = 0.0;
  for (size_t i = start; i < end; ++i) {
    maxVal = std::max(maxVal, std::abs(data[i]));
  }
  return maxVal;
}

int zeroCrossings(const std::vector<double>& signal, size_t start, size_t end) {
  auto sign = [](double v) { return (v > 0.0) - (v < 0.0); };
  int count = 0;
  for (size_t i = start + 1; i < end; ++i) {
    int s0 = sign(signal[i - 1]);
    int s1 = sign(signal[i]);
    if (s0 != 0 && s1 != 0 && s0 != s1) {
      ++count;
    }
  }
  return count;
}

}  // namespace

/** @brief Given an MP3 containing alternating 0.2/0.8 amplitude sine sections,
 * the audio filtering pipeline should return a normalized time-domain signal
 * whose peak magnitude is consistent (≈1.0) across both sections while
 * preserving frequency content (zero-crossing count). */
// Temporarily disabled as amplitude normalization is not yet implemented from MP3 input.
TEST(AudioFilteringTest, DISABLED_NormalizeAmplitudeFromMp3) {
  MP3Data data = readMP3File("audio/alt_amp_sine.mp3");
  ASSERT_FALSE(data.channel1.empty());

  const size_t total = data.channel1.size();
  const size_t mid = total / 2;

  // Inspect raw decoded signal that the filtering module consumes.
  const double maxFirst = maxAbs(data.channel1, 0, mid);
  const double maxSecond = maxAbs(data.channel1, mid, total);

  // Expected: both sections should be scaled to the same target amplitude.
  EXPECT_NEAR(maxFirst, 1.0, 1e-3);
  EXPECT_NEAR(maxSecond, 1.0, 1e-3);
  EXPECT_NEAR(maxFirst, maxSecond, 1e-3);

  // Frequency should remain unchanged by normalization: zero-crossing counts
  // stay the same between sections before/after normalization.
  const int zcFirst = zeroCrossings(data.channel1, 0, mid);
  const int zcSecond = zeroCrossings(data.channel1, mid, total);
  EXPECT_EQ(zcFirst, zcSecond);
}

