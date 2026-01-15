/**
 ******************************************************************************
 * @file    gcc_phat_test.cpp
 * @brief   Unit tests for GCC-PhaT algo.
 ******************************************************************************
 */

#include "gccPhat.h"

#include <gtest/gtest.h>

#include "angles.hpp"
#include "constants.h"
#include "mp3.h"

/** @brief Given an audio input from a single audio source from a room, assert
 * that the GCC-PhaT can esitmate the direction of sound source correctly. */
TEST(GCCPhaTTest, SingleAudioSource) {
  // Read in mp3 data for each mic..
  MP3Data mic1Data = readMP3File("audio/single_source/mic_1.mp3");
  MP3Data mic2Data = readMP3File("audio/single_source/mic_2.mp3");
  MP3Data mic3Data = readMP3File("audio/single_source/mic_3.mp3");
  MP3Data mic4Data = readMP3File("audio/single_source/mic_4.mp3");
  const int sampleFrequency = 8000;

  // Choose an arbitray window in the middle of the mp3.
  const int OFFSET = 10000;
  std::vector<float> mic1Input(
      mic1Data.channel1.begin() + OFFSET,
      mic1Data.channel1.begin() + OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic2Input(
      mic2Data.channel1.begin() + OFFSET,
      mic2Data.channel1.begin() + OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic3Input(
      mic3Data.channel1.begin() + OFFSET,
      mic3Data.channel1.begin() + OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic4Input(
      mic4Data.channel1.begin() + OFFSET,
      mic4Data.channel1.begin() + OFFSET + WAVEFORM_SAMPLES);

  // Run GCC-PhaT on the audio sample input.
  GCCPhaT gccPhat{WAVEFORM_SAMPLES, sampleFrequency};
  float angle_rad =
      gccPhat.calculateDirection(mic1Input, mic2Input, mic3Input, mic4Input);

  // Assert that the angle is approximately 315 degrees from the origin.
  float expectedAngle_rad = degreeToRad(315.0f);
  float tolerableError_rad = degreeToRad(5.0f);

  EXPECT_NEAR(angle_rad, expectedAngle_rad, tolerableError_rad);
}
