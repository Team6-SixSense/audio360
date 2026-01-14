/**
 ******************************************************************************
 * @file    ifft_test.cpp
 * @brief   Unit tests for IFFT (Inverse Fast Fourier Transform). Tests here
 *          uses a mock as hardware acceleration method are not available for CI
 *          runs.
 ******************************************************************************
 */

#include "ifft.h"

#include <gtest/gtest.h>

#include <iostream>

#include "frequencyDomain.h"


const static float PRECISION_ERROR = 0.0001;

/** @brief Given a frequency domain with equal amplitude at all frequencies,
 * the output signal is an impulse. */
TEST(IFFTTest, AllFrequenciesToImpulse) {
  // Create frequency domain with equal amplitude at all frequencies.
  uint16_t N = 17;  // This gives 32 samples which is minimum supported val.
  FrequencyDomain frequencyDomain(N);
  frequencyDomain.frequency.assign(N, 1.0f);
  frequencyDomain.real.assign(N, 1.0f);
  frequencyDomain.img.assign(N, 0.0f);
  frequencyDomain.magnitude.assign(N, 1.0f);
  frequencyDomain.powerMagnitude.assign(N, 1.0f);

  // Run IFFT.
  IFFT ifft = IFFT((N - 1) * 2);
  std::vector<float> timeDomain = ifft.frequencyToTime(frequencyDomain);

  // Assert output time signal is the impulse response.
  EXPECT_GT(timeDomain[0], PRECISION_ERROR);

  for (size_t i = 1; i < N; i++) {
    EXPECT_LT(std::abs(timeDomain[i]), PRECISION_ERROR);
  }
}
