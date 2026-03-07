/**
 ******************************************************************************
 * @file    frequency_analysis_integration_test.cpp
 * @brief   Integration tests for simultaneous classification and DOA.
 *          Tests correspond to Test-NFR5.1 in VnVPlan.
 ******************************************************************************
 */

#include "classification.h"
#include "doa.h"
#include "gccPhat.h"

#include <gtest/gtest.h>

#include <cmath>
#include <set>
#include <vector>

#include "AudioFile.h"
#include "angles.hpp"
#include "constants.h"
#include "mp3.h"
#include "test_helper.h"

/**
 * @brief Converts samples from double vector to float vector.
 * @param samples Source samples (double precision).
 * @param start Starting index.
 * @param end Ending index (exclusive).
 * @return Float vector with converted samples.
 */
static std::vector<float> ToFloatSamples(const std::vector<double>& samples,
                                         size_t start, size_t end) {
  std::vector<float> out;
  out.reserve(end - start);
  for (size_t i = start; i < end && i < samples.size(); ++i) {
    out.push_back(static_cast<float>(samples[i]));
  }
  return out;
}

/**
 * @brief Converts radians to degrees.
 * @param angleRad Angle in radians.
 * @return Angle in degrees.
 */
static float radianToDegree(float angleRad) {
  return angleRad * 180.0f / PI_32;
}

/**
 * @brief Test-NFR5.1: Simultaneous classification and direction estimation.
 * @details Verifies frequency analysis can perform both classification and
 *          DOA estimation on the same audio source.
 * @see VnVPlan.tex Section 3.2.5 (Frequency Analysis Tests)
 * @note Full 3-source test requires multi-source audio generation.
 *       This test validates single-source case.
 */
TEST(FrequencyAnalysisIntegrationTest, SimultaneousClassificationAndDOA_SingleSource) {
  // Initialize classification module
  const uint16_t numMelFilters = 6;
  const uint16_t numDCTCoeff = 6;
  const uint16_t numPCAComponents = 6;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES, numMelFilters, numDCTCoeff,
                             numPCAComponents, numClasses);

  // Initialize DOA module
  const int sampleFrequency = SAMPLE_FREQUENCY;
  GCCPhaT gccPhat{WAVEFORM_SAMPLES, sampleFrequency};

  // Load test audio from known direction and class
  // Using existing test audio: car horn from 0 degrees
  std::string folder = "audio/mic_recordings/";
  AudioFile<double> audioFile1, audioFile2, audioFile3, audioFile4;
  
  audioFile1.load(folder + "mic0_angle_0.wav");
  audioFile2.load(folder + "mic1_angle_0.wav");
  audioFile3.load(folder + "mic2_angle_0.wav");
  audioFile4.load(folder + "mic3_angle_0.wav");

  const int OFFSET = 1500;

  // Extract audio segments
  std::vector<float> mic1Input = ToFloatSamples(audioFile1.samples[0], OFFSET,
                                                  OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic2Input = ToFloatSamples(audioFile2.samples[0], OFFSET,
                                                  OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic3Input = ToFloatSamples(audioFile3.samples[0], OFFSET,
                                                  OFFSET + WAVEFORM_SAMPLES);
  std::vector<float> mic4Input = ToFloatSamples(audioFile4.samples[0], OFFSET,
                                                  OFFSET + WAVEFORM_SAMPLES);

  // Run classification (using mic1 as reference)
  classifier.Classify(mic1Input);
  std::string classLabel = classifier.getClassificationLabel();

  // Run DOA estimation
  float directionRad = gccPhat.calculateDirection(mic1Input, mic2Input,
                                                   mic3Input, mic4Input);

  // Verify both operations succeeded
  EXPECT_FALSE(classLabel.empty());
  EXPECT_GE(directionRad, 0.0f);
  EXPECT_LE(directionRad, TWO_PI_32);

  // Known direction is 0 degrees (0 radians)
  float expectedDirection = 0.0f;
  float directionError = std::fabs(directionRad - expectedDirection);

  // Test-NFR5.1 Pass Criteria: Direction error ≤ 45° (0.785 radians)
  const float MAX_DIRECTION_ERROR = degreeToRad(45.0f);
  EXPECT_LE(directionError, MAX_DIRECTION_ERROR);

  // Note: Classification accuracy threshold (≥ 90%) tested separately
  // in ClassificationTest suite. Here we verify both can run simultaneously.

  std::cout << "Integration test results:" << std::endl;
  std::cout << "  Classification: " << classLabel << std::endl;
  std::cout << "  Direction: " << directionRad << " rad ("
            << radianToDegree(directionRad) << "°)" << std::endl;
  std::cout << "  Direction error: " << radianToDegree(directionError) << "°"
            << std::endl;
}

/**
 * @brief Test classification and DOA from multiple angles.
 */
TEST(FrequencyAnalysisIntegrationTest, MultipleAngles) {
  Classification classifier(WAVEFORM_SAMPLES, 6, 6, 6, 3);
  GCCPhaT gccPhat{WAVEFORM_SAMPLES, SAMPLE_FREQUENCY};

  std::vector<int> angles = {0, 45, 90, 135, 180};

  for (int angle : angles) {
    std::string folder = "audio/mic_recordings/";
    AudioFile<double> audioFile1, audioFile2, audioFile3, audioFile4;
    
    audioFile1.load(folder + "mic0_angle_" + std::to_string(angle) + ".wav");
    audioFile2.load(folder + "mic1_angle_" + std::to_string(angle) + ".wav");
    audioFile3.load(folder + "mic2_angle_" + std::to_string(angle) + ".wav");
    audioFile4.load(folder + "mic3_angle_" + std::to_string(angle) + ".wav");

    const int OFFSET = 1500;

    std::vector<float> mic1 = ToFloatSamples(audioFile1.samples[0], OFFSET,
                                              OFFSET + WAVEFORM_SAMPLES);
    std::vector<float> mic2 = ToFloatSamples(audioFile2.samples[0], OFFSET,
                                              OFFSET + WAVEFORM_SAMPLES);
    std::vector<float> mic3 = ToFloatSamples(audioFile3.samples[0], OFFSET,
                                              OFFSET + WAVEFORM_SAMPLES);
    std::vector<float> mic4 = ToFloatSamples(audioFile4.samples[0], OFFSET,
                                              OFFSET + WAVEFORM_SAMPLES);

    // Run both simultaneously
    classifier.Classify(mic1);
    float direction = gccPhat.calculateDirection(mic1, mic2, mic3, mic4);

    // Verify both succeed
    EXPECT_FALSE(classifier.getClassificationLabel().empty());
    EXPECT_GE(direction, 0.0f);
    EXPECT_LE(direction, TWO_PI_32);

    // Verify direction accuracy
    float expectedRad = degreeToRad(static_cast<float>(angle));
    float error = std::fabs(normalizeAngleRad(direction - expectedRad));
    EXPECT_LE(error, degreeToRad(45.0f))
        << "Angle: " << angle << "°, Direction: " << radianToDegree(direction)
        << "°, Error: " << radianToDegree(error) << "°";
  }
}

/**
 * @brief Test performance of simultaneous classification and DOA.
 */
TEST(FrequencyAnalysisIntegrationTest, SimultaneousProcessingPerformance) {
  Classification classifier(WAVEFORM_SAMPLES, 6, 6, 6, 3);
  GCCPhaT gccPhat{WAVEFORM_SAMPLES, SAMPLE_FREQUENCY};

  // Load test audio
  AudioFile<double> audioFile1, audioFile2, audioFile3, audioFile4;
  audioFile1.load("audio/mic_recordings/mic0_angle_0.wav");
  audioFile2.load("audio/mic_recordings/mic1_angle_0.wav");
  audioFile3.load("audio/mic_recordings/mic2_angle_0.wav");
  audioFile4.load("audio/mic_recordings/mic3_angle_0.wav");

  std::vector<float> mic1 = ToFloatSamples(audioFile1.samples[0], 1500,
                                            1500 + WAVEFORM_SAMPLES);
  std::vector<float> mic2 = ToFloatSamples(audioFile2.samples[0], 1500,
                                            1500 + WAVEFORM_SAMPLES);
  std::vector<float> mic3 = ToFloatSamples(audioFile3.samples[0], 1500,
                                            1500 + WAVEFORM_SAMPLES);
  std::vector<float> mic4 = ToFloatSamples(audioFile4.samples[0], 1500,
                                            1500 + WAVEFORM_SAMPLES);

  // Measure combined processing time
  auto start = std::chrono::high_resolution_clock::now();

  classifier.Classify(mic1);
  float direction = gccPhat.calculateDirection(mic1, mic2, mic3, mic4);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;

  std::cout << "Simultaneous classification + DOA: " << duration.count()
            << " ms" << std::endl;

  // Verify both completed
  EXPECT_FALSE(classifier.getClassificationLabel().empty());
  EXPECT_GE(direction, 0.0f);
  EXPECT_LE(direction, TWO_PI_32);

  // Note: Real-time requirement (< 62.5 μs per frame) requires hardware testing
}
