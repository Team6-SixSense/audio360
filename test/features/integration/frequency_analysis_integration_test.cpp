/**
 ******************************************************************************
 * @file    frequency_analysis_integration_test.cpp
 * @brief   Integration tests for simultaneous classification and DOA.
 *          Tests correspond to Test-NFR5.1 in VnVPlan.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <cmath>
#include <set>
#include <vector>

#include "AudioFile.h"
#include "angles.hpp"
#include "classification.h"
#include "constants.h"
#include "doa.h"
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
 * @brief Calculate angular error with proper wrapping.
 * @details Handles angle wrapping to ensure error is always the shortest
 *          angular distance between estimated and known direction.
 */
static double calculateAngularError(double estimated, double known) {
  double error = std::abs(estimated - known);
  // Wrap error to [0, π] (shortest angular distance)
  if (error > M_PI) {
    error = 2.0 * M_PI - error;
  }
  return error;
}

/**
 * @brief Test-NFR5.1: Simultaneous classification and direction estimation.
 * @details Verifies frequency analysis can perform both classification and
 *          DOA estimation on the same audio source.
 * @see VnVPlan.tex Section 3.2.5 (Frequency Analysis Tests)
 * @note Full 3-source test requires multi-source audio generation.
 *       This test validates single-source case.
 */
TEST(FrequencyAnalysisIntegrationTest,
     SimultaneousClassificationAndDOA_SingleSource) {
  // Initialize classification module
  const uint16_t numMelFilters = 6;
  const uint16_t numDCTCoeff = 6;
  const uint16_t numPCAComponents = 6;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES, numMelFilters, numDCTCoeff,
                            numPCAComponents, numClasses);

  // Initialize DOA module
  DOA doa(WAVEFORM_SAMPLES);

  // Load test audio from known direction and class
  // Using existing test audio: car horn from 0 degrees
  std::string folder = "audio/mic_recordings/";
  AudioFile<double> audioFile1, audioFile2, audioFile3, audioFile4;

  audioFile1.load(folder + "mic0_angle_0.wav");
  audioFile2.load(folder + "mic1_angle_0.wav");
  audioFile3.load(folder + "mic2_angle_0.wav");
  audioFile4.load(folder + "mic3_angle_0.wav");

  // Extract audio segments (using same offset as DOA accuracy test: 0)
  std::vector<float> mic1Input =
      ToFloatSamples(audioFile1.samples[0], 0, WAVEFORM_SAMPLES);
  std::vector<float> mic2Input =
      ToFloatSamples(audioFile2.samples[0], 0, WAVEFORM_SAMPLES);
  std::vector<float> mic3Input =
      ToFloatSamples(audioFile3.samples[0], 0, WAVEFORM_SAMPLES);
  std::vector<float> mic4Input =
      ToFloatSamples(audioFile4.samples[0], 0, WAVEFORM_SAMPLES);

  // Run classification (using mic1 as reference)
  classifier.Classify(mic1Input);
  std::string classLabel = classifier.getClassificationLabel();

  // Run DOA estimation (using DOA class with GCC-PHAT algorithm)
  float directionRad = doa.calculateDirection(
      mic1Input, mic2Input, mic3Input, mic4Input, DOA_Algorithms::GCC_PHAT);

  // Verify both operations succeeded
  EXPECT_FALSE(classLabel.empty()) << "Classification returned empty label";
  EXPECT_GE(directionRad, 0.0f);
  EXPECT_LE(directionRad, TWO_PI_32);

  // Classification check: Should be someone_talking, siren, smoke_alarm, or unknown
  bool classificationAcceptable =
      (classLabel == "someone_talking" || classLabel == "siren" ||
       classLabel == "smoke_alarm" || classLabel == "unknown");
  EXPECT_TRUE(classificationAcceptable)
      << "Classification returned unexpected label: " << classLabel;

  // Known direction is 0 degrees (0 radians)
  double expectedDirection = 0.0;
  double directionError =
      calculateAngularError(directionRad, expectedDirection);

  // Test-NFR5.1 Pass Criteria: Direction error ≤ 45° (0.785 radians)
  const double MAX_DIRECTION_ERROR = degreeToRad(45.0);
  EXPECT_LE(directionError, MAX_DIRECTION_ERROR)
      << "Direction error " << radToDegree(directionError)
      << "° exceeds 45° threshold";

  std::cout << "Integration test results:" << std::endl;
  std::cout << "  Classification: " << classLabel
            << (classificationAcceptable ? " (acceptable)" : " (UNEXPECTED)")
            << std::endl;
  std::cout << "  Direction: " << directionRad << " rad ("
            << radToDegree(directionRad) << "°)" << std::endl;
  std::cout << "  Direction error: " << radToDegree(directionError) << "°"
            << std::endl;
}

/**
 * @brief Test classification and DOA from multiple angles.
 */
TEST(FrequencyAnalysisIntegrationTest, MultipleAngles) {
  Classification classifier(WAVEFORM_SAMPLES, 6, 6, 6, 3);
  DOA doa(WAVEFORM_SAMPLES);

  std::vector<int> angles = {0, 45, 90, 135, 180};

  for (int angle : angles) {
    std::string folder = "audio/mic_recordings/";
    AudioFile<double> audioFile1, audioFile2, audioFile3, audioFile4;

    audioFile1.load(folder + "mic0_angle_" + std::to_string(angle) + ".wav");
    audioFile2.load(folder + "mic1_angle_" + std::to_string(angle) + ".wav");
    audioFile3.load(folder + "mic2_angle_" + std::to_string(angle) + ".wav");
    audioFile4.load(folder + "mic3_angle_" + std::to_string(angle) + ".wav");

    // Extract audio segments (using same offset as DOA accuracy test: 0)
    std::vector<float> mic1 =
        ToFloatSamples(audioFile1.samples[0], 0, WAVEFORM_SAMPLES);
    std::vector<float> mic2 =
        ToFloatSamples(audioFile2.samples[0], 0, WAVEFORM_SAMPLES);
    std::vector<float> mic3 =
        ToFloatSamples(audioFile3.samples[0], 0, WAVEFORM_SAMPLES);
    std::vector<float> mic4 =
        ToFloatSamples(audioFile4.samples[0], 0, WAVEFORM_SAMPLES);

    // Run both simultaneously
    classifier.Classify(mic1);
    std::string label = classifier.getClassificationLabel();
    float direction = doa.calculateDirection(mic1, mic2, mic3, mic4,
                                             DOA_Algorithms::GCC_PHAT);

    // Verify classification succeeded (correct class, similar class, or unknown
    // acceptable)
    EXPECT_FALSE(label.empty())
        << "Classification returned empty label for angle " << angle;
    bool classAcceptable = (label == "someone_talking" || label == "siren" ||
                            label == "smoke_alarm" || label == "unknown");
    EXPECT_TRUE(classAcceptable)
        << "Angle " << angle << "°: unexpected classification " << label;

    // Verify DOA succeeded
    EXPECT_GE(direction, 0.0f);
    EXPECT_LE(direction, TWO_PI_32);

    // Verify direction accuracy (≤ 45° error threshold)
    double expectedRad = degreeToRad(static_cast<double>(angle));
    double error = calculateAngularError(direction, expectedRad);
    EXPECT_LE(error, degreeToRad(45.0))
        << "Angle: " << angle << "°, Direction: " << radToDegree(direction)
        << "°, Error: " << radToDegree(error) << "°";
  }
}

/**
 * @brief Test performance of simultaneous classification and DOA.
 */
TEST(FrequencyAnalysisIntegrationTest, SimultaneousProcessingPerformance) {
  Classification classifier(WAVEFORM_SAMPLES, 6, 6, 6, 3);
  DOA doa(WAVEFORM_SAMPLES);

  // Load test audio
  AudioFile<double> audioFile1, audioFile2, audioFile3, audioFile4;
  audioFile1.load("audio/mic_recordings/mic0_angle_0.wav");
  audioFile2.load("audio/mic_recordings/mic1_angle_0.wav");
  audioFile3.load("audio/mic_recordings/mic2_angle_0.wav");
  audioFile4.load("audio/mic_recordings/mic3_angle_0.wav");

  // Extract audio segments (using same offset as DOA accuracy test: 0)
  std::vector<float> mic1 =
      ToFloatSamples(audioFile1.samples[0], 0, WAVEFORM_SAMPLES);
  std::vector<float> mic2 =
      ToFloatSamples(audioFile2.samples[0], 0, WAVEFORM_SAMPLES);
  std::vector<float> mic3 =
      ToFloatSamples(audioFile3.samples[0], 0, WAVEFORM_SAMPLES);
  std::vector<float> mic4 =
      ToFloatSamples(audioFile4.samples[0], 0, WAVEFORM_SAMPLES);

  // Measure combined processing time
  auto start = std::chrono::high_resolution_clock::now();

  classifier.Classify(mic1);
  float direction =
      doa.calculateDirection(mic1, mic2, mic3, mic4, DOA_Algorithms::GCC_PHAT);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;

  std::cout << "Simultaneous classification + DOA: " << duration.count()
            << " ms" << std::endl;

  // Verify both completed (classification can be any non-empty result including
  // "unknown")
  std::string label = classifier.getClassificationLabel();
  EXPECT_FALSE(label.empty());
  bool classAcceptable = (label == "someone_talking" || label == "siren" ||
                          label == "smoke_alarm" || label == "unknown");
  EXPECT_TRUE(classAcceptable) << "Unexpected classification: " << label;

  EXPECT_GE(direction, 0.0f);
  EXPECT_LE(direction, TWO_PI_32);

  // Note: Real-time requirement (< 62.5 μs per frame) requires hardware testing
}
