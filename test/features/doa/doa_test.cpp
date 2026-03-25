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
#include "exceptions.hpp"
#include "test_helper.h"

using namespace ::testing;

/** @brief Given 4 random microphone data, valid angle ranges are returned by
 * GCC PhaT DoA algorithm. */
TEST(DOATest, gccPhatAngleRange) {
  // Create random microphone data.
  size_t numSamples = 32;
  float mic1[numSamples];
  float mic2[numSamples];
  float mic3[numSamples];
  float mic4[numSamples];

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

/** @brief Given 4 random microphone data and no supported doa algo is
 * requested, assert audio processing failure is thrown. */
TEST(DOATest, NoDoAAlgo) {
  // Create random microphone data.
  size_t numSamples = 4;
  float mic1[numSamples];
  float mic2[numSamples];
  float mic3[numSamples];
  float mic4[numSamples];

  for (size_t i = 0; i < numSamples; i++) {
    mic1[i] = generateRandomFloat32();
    mic2[i] = generateRandomFloat32();
    mic3[i] = generateRandomFloat32();
    mic4[i] = generateRandomFloat32();
  }

  // Run DOA.
  DOA doa = DOA(numSamples);
  EXPECT_THROW(
      doa.calculateDirection(mic1, mic2, mic3, mic4, DOA_Algorithms::NONE),
      AudioProcessingException);
}
