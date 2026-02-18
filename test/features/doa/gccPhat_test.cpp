/**
 ******************************************************************************
 * @file    gcc_phat_test.cpp
 * @brief   Unit tests for GCC-PhaT algo.
 ******************************************************************************
 */

#include "gccPhat.h"

#include <gtest/gtest.h>

#include "AudioFile.h"
#include "angles.hpp"
#include "constants.h"

const float TOLERABLE_ERROR_RAD = degreeToRad(5.0f);

/** @brief Struct for parameterized testing. */
struct GCCPhatAngleParamType {
  int angle;  // Angle in degree with 0 degrees being North and 90 to West.
};

/** @brief Parameterized test class for GCC PhaT angle accuracy. */
class GCCPhatAngleTest
    : public ::testing::TestWithParam<GCCPhatAngleParamType> {};

/** @brief Given an audio input from a single audio source from a room, assert
 * that the GCC-PhaT can estimate the direction of sound source correctly. */
TEST_P(GCCPhatAngleTest, SingleAudioSourceAngle) {
  GCCPhatAngleParamType param = GetParam();
  int angle = param.angle;

  // Read in audio data for each mic.
  std::string folder = "audio/mic_recordings/";
  AudioFile<double> audioFile1(folder + "mic1_angle_" + std::to_string(angle) +
                               ".wav");
  AudioFile<double> audioFile2(folder + "mic2_angle_" + std::to_string(angle) +
                               ".wav");
  AudioFile<double> audioFile3(folder + "mic3_angle_" + std::to_string(angle) +
                               ".wav");
  AudioFile<double> audioFile4(folder + "mic4_angle_" + std::to_string(angle) +
                               ".wav");

  const int sampleFrequency = audioFile1.getSampleRate();

  // Choose an arbitray window in the middle of the audio data.
  const int OFFSET = 1500;
  std::vector<float> mic1Input(
      audioFile1.samples[0].begin() + OFFSET,
      audioFile1.samples[0].begin() + OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic2Input(
      audioFile2.samples[0].begin() + OFFSET,
      audioFile2.samples[0].begin() + OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic3Input(
      audioFile3.samples[0].begin() + OFFSET,
      audioFile3.samples[0].begin() + OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic4Input(
      audioFile4.samples[0].begin() + OFFSET,
      audioFile4.samples[0].begin() + OFFSET + WAVEFORM_SAMPLES);

  // Run GCC-PhaT on the audio sample input.
  GCCPhaT gccPhat{WAVEFORM_SAMPLES, sampleFrequency};
  float angle_rad =
      gccPhat.calculateDirection(mic1Input, mic2Input, mic3Input, mic4Input);

  // Assert that the angle is approximately 0 degrees from the origin.
  float expectedAngle_rad = degreeToRad(static_cast<float>(angle));
  EXPECT_NEAR(angle_rad, expectedAngle_rad, TOLERABLE_ERROR_RAD);
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(
    AngleValues, GCCPhatAngleTest,
    ::testing::Values(GCCPhatAngleParamType{0}, GCCPhatAngleParamType{45},
                      GCCPhatAngleParamType{90}, GCCPhatAngleParamType{135},
                      GCCPhatAngleParamType{180}, GCCPhatAngleParamType{225},
                      GCCPhatAngleParamType{270}, GCCPhatAngleParamType{315}));
