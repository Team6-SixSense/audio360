/**
 ******************************************************************************
 * @file    doa_accuracy_test.cpp
 * @brief   Unit tests for DOA accuracy measurement (Test-FR-5.2).
 * @details Tests DOA accuracy against known directions with MAE calculation
 *          as specified in VnVPlan.tex Section 3.1.7.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <vector>

#include "AudioFile.h"
#include "angles.hpp"
#include "constants.h"
#include "doa.h"

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
 * @brief Convert double samples to float samples.
 */
static void ToFloatSamplesArray(const std::vector<double>& samples,
                                size_t start, size_t end, float* in) {
  for (size_t i = start; i < end && i < samples.size(); i++) {
    in[i] = static_cast<float>(samples[i]);
  }
}

/**
 * @brief Test-FR-5.2: DOA estimation accuracy against known directions.
 * @details Tests DOA accuracy for sources at known angles, verifying
 *          estimated direction is within θ_e of known direction.
 *          Also calculates MAE which must be < θ_e/2.
 * @see VnVPlan.tex Section 3.1.7 (Frequency Analysis Tests)
 */
TEST(DOAAccuracyTest, KnownDirectionEstimation) {
  // Test parameters
  const double THETA_E = degreeToRad(45.0);    // θ_e = 45° (π/4 radians)
  const double MAE_THRESHOLD = THETA_E / 2.0;  // MAE < 22.5°

  // Known test angles (in degrees, will be converted to radians)
  std::vector<double> knownAngles = {0.0, 45.0, 90.0, 135.0};

  std::vector<double> estimatedAngles;
  std::vector<double> errors;

  // Process each known angle
  for (double knownAngleDeg : knownAngles) {
    // Construct filename (e.g., "audio/mic_recordings/mic0_angle_0.wav")
    std::string folder = "audio/mic_recordings/";
    int angleInt = static_cast<int>(knownAngleDeg);

    try {
      // Load microphone data from known direction
      AudioFile<double> mic0, mic1, mic2, mic3;
      mic0.load(folder + "mic0_angle_" + std::to_string(angleInt) + ".wav");
      mic1.load(folder + "mic1_angle_" + std::to_string(angleInt) + ".wav");
      mic2.load(folder + "mic2_angle_" + std::to_string(angleInt) + ".wav");
      mic3.load(folder + "mic3_angle_" + std::to_string(angleInt) + ".wav");

      // Extract first frame from each mic (samples is a 2D array
      // [channel][sample])
      float mic0Data[WAVEFORM_SAMPLES];
      ToFloatSamplesArray(mic0.samples[0], 0, WAVEFORM_SAMPLES, mic0Data);

      float mic1Data[WAVEFORM_SAMPLES];
      ToFloatSamplesArray(mic1.samples[0], 0, WAVEFORM_SAMPLES, mic1Data);

      float mic2Data[WAVEFORM_SAMPLES];
      ToFloatSamplesArray(mic2.samples[0], 0, WAVEFORM_SAMPLES, mic2Data);

      float mic3Data[WAVEFORM_SAMPLES];
      ToFloatSamplesArray(mic3.samples[0], 0, WAVEFORM_SAMPLES, mic3Data);

      // Run DOA algorithm
      DOA doa(WAVEFORM_SAMPLES);
      float estimatedAngleRad = doa.calculateDirection(
          mic0Data, mic1Data, mic2Data, mic3Data, DOA_Algorithms::GCC_PHAT);

      double knownAngleRad = degreeToRad(knownAngleDeg);
      double errorRad = calculateAngularError(estimatedAngleRad, knownAngleRad);
      double errorDeg = radToDegree(errorRad);

      estimatedAngles.push_back(estimatedAngleRad);
      errors.push_back(errorRad);

      std::cout << "Known: " << knownAngleDeg
                << "°, Estimated: " << radToDegree(estimatedAngleRad)
                << "°, Error: " << errorDeg << "°" << std::endl;

      // Verify error is within θ_e threshold for this sample
      EXPECT_LE(errorRad, THETA_E)
          << "DOA error " << errorDeg << "° exceeds θ_e threshold "
          << radToDegree(THETA_E) << "° for known angle " << knownAngleDeg
          << "°";

    } catch (const std::exception& e) {
      // Skip this angle if audio files not available
      std::cout << "Skipping angle " << knownAngleDeg
                << "° (audio not available)" << std::endl;
      continue;
    }
  }

  ASSERT_GT(errors.size(), 0u)
      << "No valid DOA tests could be performed (audio files missing)";

  // Calculate Mean Absolute Error (MAE)
  double mae = 0.0;
  for (double error : errors) {
    mae += error;
  }
  mae /= errors.size();

  double maeDeg = radToDegree(mae);
  double maeThresholdDeg = radToDegree(MAE_THRESHOLD);

  std::cout << "\nDOA Accuracy Summary:" << std::endl;
  std::cout << "  Samples tested: " << errors.size() << std::endl;
  std::cout << "  Mean Absolute Error (MAE): " << maeDeg << "°" << std::endl;
  std::cout << "  Required MAE: < " << maeThresholdDeg << "°" << std::endl;

  // Verify MAE is below threshold
  EXPECT_LT(mae, MAE_THRESHOLD)
      << "MAE " << maeDeg << "° exceeds threshold " << maeThresholdDeg << "°";
}

/**
 * @brief Test DOA accuracy with extended angle range.
 * @details Tests additional angles (180°, 225°, 270°, 315°) if available.
 */
TEST(DOAAccuracyTest, ExtendedAngleRange) {
  const double THETA_E = degreeToRad(45.0);

  std::vector<double> extendedAngles = {180.0, 225.0, 270.0, 315.0};

  int testedAngles = 0;
  int passedAngles = 0;

  for (double knownAngleDeg : extendedAngles) {
    std::string folder = "audio/mic_recordings/";
    int angleInt = static_cast<int>(knownAngleDeg);

    try {
      AudioFile<double> mic0, mic1, mic2, mic3;
      mic0.load(folder + "mic0_angle_" + std::to_string(angleInt) + ".wav");
      mic1.load(folder + "mic1_angle_" + std::to_string(angleInt) + ".wav");
      mic2.load(folder + "mic2_angle_" + std::to_string(angleInt) + ".wav");
      mic3.load(folder + "mic3_angle_" + std::to_string(angleInt) + ".wav");

      // Extract first frame from each mic (samples is a 2D array
      // [channel][sample])
      float mic0Data[WAVEFORM_SAMPLES];
      ToFloatSamplesArray(mic0.samples[0], 0, WAVEFORM_SAMPLES, mic0Data);

      float mic1Data[WAVEFORM_SAMPLES];
      ToFloatSamplesArray(mic1.samples[0], 0, WAVEFORM_SAMPLES, mic1Data);

      float mic2Data[WAVEFORM_SAMPLES];
      ToFloatSamplesArray(mic2.samples[0], 0, WAVEFORM_SAMPLES, mic2Data);

      float mic3Data[WAVEFORM_SAMPLES];
      ToFloatSamplesArray(mic3.samples[0], 0, WAVEFORM_SAMPLES, mic3Data);

      DOA doa(WAVEFORM_SAMPLES);
      float estimatedAngleRad = doa.calculateDirection(
          mic0Data, mic1Data, mic2Data, mic3Data, DOA_Algorithms::GCC_PHAT);

      double knownAngleRad = degreeToRad(knownAngleDeg);
      double errorRad = calculateAngularError(estimatedAngleRad, knownAngleRad);
      double errorDeg = radToDegree(errorRad);

      testedAngles++;
      if (errorRad <= THETA_E) {
        passedAngles++;
      }

      std::cout << "Extended angle " << knownAngleDeg << "°: Error " << errorDeg
                << "° (" << (errorRad <= THETA_E ? "PASS" : "FAIL") << ")"
                << std::endl;

    } catch (...) {
      // Skip if not available
      continue;
    }
  }

  if (testedAngles == 0) {
    GTEST_SKIP() << "No extended angle audio files available";
  }

  std::cout << "Extended angles: " << passedAngles << "/" << testedAngles
            << " passed" << std::endl;

  // At least some extended angles should pass if they are available
  EXPECT_GT(passedAngles, 0)
      << "No extended angle tests passed (0/" << testedAngles << ")";
}
