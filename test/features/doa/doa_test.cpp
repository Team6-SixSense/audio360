/**
 ******************************************************************************
 * @file    doa_test.cpp
 * @brief   Unit tests for Doa controller class.
 ******************************************************************************
 */

#include "doa.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

#include "constants.h"
#include "test_helper.h"

using namespace ::testing;

/** @brief Given 4 random microphone data, valid angle ranges are returned by
 * GCC PhaT DoA algorithm. */
TEST(DOATest, gccPhatAngleRange) {
  // Create random microphone data.
  size_t numSamples = 32;
  std::vector<float> mic1(numSamples);
  std::vector<float> mic2(numSamples);
  std::vector<float> mic3(numSamples);
  std::vector<float> mic4(numSamples);

  for (size_t i = 0; i < numSamples; i++) {
    mic1[i] = generateRandomFloat32();
    mic2[i] = generateRandomFloat32();
    mic3[i] = generateRandomFloat32();
    mic4[i] = generateRandomFloat32();
  }

  // Run DOA.
  DOA doa = DOA(numSamples);
  float angle =
      doa.calculateDirection(mic1, mic2, mic3, mic4, DOA_Algorithms::GCC_PHAT);

  // Assert angle is within valid range.
  EXPECT_THAT(angle, AllOf(Ge(0), Le(TWO_PI_32)));
}

/** @brief Given 4 random microphone data, default angle is return when no DoA
 * algorithm is used. */
TEST(DOATest, NoDoAAlgo) {
  // Create random microphone data.
  size_t numSamples = 4;
  std::vector<float> mic1(numSamples);
  std::vector<float> mic2(numSamples);
  std::vector<float> mic3(numSamples);
  std::vector<float> mic4(numSamples);

  for (size_t i = 0; i < numSamples; i++) {
    mic1[i] = generateRandomFloat32();
    mic2[i] = generateRandomFloat32();
    mic3[i] = generateRandomFloat32();
    mic4[i] = generateRandomFloat32();
  }

  // Run DOA.
  DOA doa = DOA(numSamples);
  float angle =
      doa.calculateDirection(mic1, mic2, mic3, mic4, DOA_Algorithms::NONE);

  // Assert angle is within valid range.
  EXPECT_NEAR(angle, 0.0, PRECISION_ERROR);
}

TEST(DOATest, IncorrectMicDataSize) {
  // Create microphone data.
  size_t numSamples = 4;
  std::vector<float> mic1(numSamples);
  std::vector<float> mic2(numSamples);
  std::vector<float> mic3(numSamples);
  std::vector<float> mic4(numSamples);

  DOA doa = DOA(numSamples);

  // Set mic 1 to different size and assert audio processing failure.
  size_t newSize = 2;
  mic1.resize(newSize);
  EXPECT_THROW(doa.calculateDirection(mic1, mic2, mic3, mic4),
               std::logic_error);
  mic1.resize(numSamples);

  // Set mic 2 to different size and assert audio processing failure.
  mic2.resize(newSize);
  EXPECT_THROW(doa.calculateDirection(mic1, mic2, mic3, mic4),
               std::logic_error);
  mic2.resize(numSamples);

  // Set mic 3 to different size and assert audio processing failure.
  mic3.resize(newSize);
  EXPECT_THROW(doa.calculateDirection(mic1, mic2, mic3, mic4),
               std::logic_error);
  mic3.resize(numSamples);

  // Set mic 4 to different size and assert audio processing failure.
  mic4.resize(newSize);
  EXPECT_THROW(doa.calculateDirection(mic1, mic2, mic3, mic4),
               std::logic_error);
  mic4.resize(numSamples);
}
