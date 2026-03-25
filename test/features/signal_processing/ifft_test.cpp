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

class IFFTTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create frequency domain with equal amplitude at all frequencies.
    frequencyDomain.N = N;
    for (int i = 0; i < N; i++) {
      frequencyDomain.real[i] = 1.0f;
      frequencyDomain.img[i] = 0.0f;
      frequencyDomain.magnitude[i] = 1.0f;
      frequencyDomain.powerMagnitude[i] = 1.0f;
      frequencyDomain.frequency[i] = 1.0f;
    }
  }

  uint16_t N = 17;  // This gives 32 samples which is the min. supported val.

  FrequencyDomain frequencyDomain = {};
};

/** @brief Given a frequency domain with equal amplitude at all frequencies,
 * the output signal is an impulse. */
TEST_F(IFFTTest, AllFrequenciesToImpulse) {
  // Run IFFT.
  IFFT ifft = IFFT((N - 1) * 2);
  size_t out_size = 0;
  float* timeDomain = ifft.frequencyToTime(frequencyDomain, out_size);

  // Assert output time signal is the impulse response.
  EXPECT_GT(timeDomain[0], PRECISION_ERROR);

  for (size_t i = 1; i < N; i++) {
    EXPECT_LT(std::abs(timeDomain[i]), PRECISION_ERROR);
  }
}

/** @brief Given a frequency domain with equal amplitude at all frequencies,
 * the output signal is an impulse. Verify on copy IFFT. */
TEST_F(IFFTTest, AllFrequenciesToImpulseCopy) {
  // Run IFFT.
  IFFT ifft = IFFT((N - 1) * 2);
  IFFT copyIfft(ifft);
  size_t out_size = 0;
  float* timeDomain = copyIfft.frequencyToTime(frequencyDomain, out_size);

  // Assert output time signal is the impulse response.
  EXPECT_GT(timeDomain[0], PRECISION_ERROR);

  for (size_t i = 1; i < N; i++) {
    EXPECT_LT(std::abs(timeDomain[i]), PRECISION_ERROR);
  }
}

/** @brief Given a frequency domain with equal amplitude at all frequencies,
 * the output signal is an impulse. Verify on assignment IFFT. */
TEST_F(IFFTTest, AllFrequenciesToImpulseAssignment) {
  // Run IFFT.
  size_t numSamples = (N - 1) * 2;
  IFFT ifft = IFFT(numSamples);
  IFFT assignmentIfft(numSamples + 1);  // Plus one to test size re-adjustment.
  assignmentIfft = ifft;
  size_t out_size = 0;
  float* timeDomain = assignmentIfft.frequencyToTime(frequencyDomain, out_size);

  // Assert output time signal is the impulse response.
  EXPECT_GT(timeDomain[0], PRECISION_ERROR);

  for (size_t i = 1; i < N; i++) {
    EXPECT_LT(std::abs(timeDomain[i]), PRECISION_ERROR);
  }
}
