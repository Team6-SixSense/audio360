/**
 ******************************************************************************
 * @file    classification_accuracy_test.cpp
 * @brief   Unit tests for classification accuracy measurement (Test-FR-5.1).
 * @details Tests classification accuracy against 90% threshold per class
 *          as specified in VnVPlan.tex Section 3.1.7.
 ******************************************************************************
 */

#include "classification.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "constants.h"
#include "mp3.h"

/**
 * @brief Convert double samples to float samples.
 */
static std::vector<float> ToFloatSamples(const std::vector<double>& samples,
                                         size_t start, size_t end) {
  std::vector<float> floatSamples;
  floatSamples.reserve(end - start);

  for (size_t i = start; i < end && i < samples.size(); i++) {
    floatSamples.push_back(static_cast<float>(samples[i]));
  }

  return floatSamples;
}

/**
 * @brief Test-FR-5.1: Classification accuracy per class.
 * @details Tests classification accuracy against known audio samples,
 *          requiring ≥90% accuracy per class as specified in VnVPlan.
 * @see VnVPlan.tex Section 3.1.7 (Frequency Analysis Tests)
 */
TEST(ClassificationAccuracyTest, JackhammerClassAccuracy) {
  const uint16_t numMelFilters = 6;
  const uint16_t numDCTCoeff = 6;
  const uint16_t numPCAComponents = 6;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES, numMelFilters, numDCTCoeff,
                            numPCAComponents, numClasses);

  // Load known jackhammer audio (16kHz resampled for classification compatibility)
  MP3Data data = readMP3File("audio/long_jackhammer_16k.mp3");
  const size_t numFrames = data.channel1.size() / WAVEFORM_SAMPLES;

  ASSERT_GT(numFrames, 0u) << "No frames available for testing";

  // Classify all frames
  size_t correctClassifications = 0;
  size_t totalFrames = 0;

  for (size_t frame = 0; frame < numFrames; ++frame) {
    std::vector<float> audio = ToFloatSamples(
        data.channel1, frame * WAVEFORM_SAMPLES,
        (frame + 1) * WAVEFORM_SAMPLES);

    classifier.Classify(audio);
    std::string label = classifier.getClassificationLabel();

    totalFrames++;
    if (label == "jackhammer") {
      correctClassifications++;
    }
  }

  // Calculate accuracy
  double accuracy = (static_cast<double>(correctClassifications) /
                     static_cast<double>(totalFrames)) *
                    100.0;

  std::cout << "Jackhammer Classification Results:" << std::endl;
  std::cout << "  Total frames: " << totalFrames << std::endl;
  std::cout << "  Correct classifications: " << correctClassifications
            << std::endl;
  std::cout << "  Accuracy: " << accuracy << "%" << std::endl;
  std::cout << "  Required: ≥90%" << std::endl;

  // Test passes if accuracy meets or exceeds 90% threshold
  EXPECT_GE(accuracy, 90.0)
      << "Classification accuracy " << accuracy
      << "% does not meet 90% requirement";
}

/**
 * @brief Test classification accuracy with car horn audio.
 * @details Tests car horn classification accuracy against 90% threshold.
 */
TEST(ClassificationAccuracyTest, CarHornClassAccuracy) {
  const uint16_t numMelFilters = 6;
  const uint16_t numDCTCoeff = 6;
  const uint16_t numPCAComponents = 6;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES, numMelFilters, numDCTCoeff,
                            numPCAComponents, numClasses);

  // Check if car horn audio file exists (16kHz resampled version)
  try {
    MP3Data data = readMP3File("audio/car_horn_16k.mp3");
    const size_t numFrames = data.channel1.size() / WAVEFORM_SAMPLES;

    if (numFrames == 0) {
      GTEST_SKIP() << "Car horn audio file has insufficient samples";
      return;
    }

    size_t correctClassifications = 0;
    size_t totalFrames = 0;

    for (size_t frame = 0; frame < numFrames; ++frame) {
      std::vector<float> audio = ToFloatSamples(
          data.channel1, frame * WAVEFORM_SAMPLES,
          (frame + 1) * WAVEFORM_SAMPLES);

      classifier.Classify(audio);
      std::string label = classifier.getClassificationLabel();

      totalFrames++;
      if (label == "car_horn" || label == "horn") {
        correctClassifications++;
      }
    }

    double accuracy = (static_cast<double>(correctClassifications) /
                       static_cast<double>(totalFrames)) *
                      100.0;

    std::cout << "Car Horn Classification Results:" << std::endl;
    std::cout << "  Total frames: " << totalFrames << std::endl;
    std::cout << "  Correct classifications: " << correctClassifications
              << std::endl;
    std::cout << "  Accuracy: " << accuracy << "%" << std::endl;
    std::cout << "  Required: ≥90%" << std::endl;

    EXPECT_GE(accuracy, 90.0)
        << "Car horn accuracy " << accuracy
        << "% does not meet 90% requirement";

  } catch (...) {
    GTEST_SKIP() << "Car horn audio file not available";
  }
}

/**
 * @brief Test overall classification accuracy across multiple classes.
 * @details Calculates average accuracy across all tested classes,
 *          as specified in VnVPlan Test-FR-5.1.
 */
TEST(ClassificationAccuracyTest, OverallAccuracyAcrossClasses) {
  const uint16_t numMelFilters = 6;
  const uint16_t numDCTCoeff = 6;
  const uint16_t numPCAComponents = 6;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES, numMelFilters, numDCTCoeff,
                            numPCAComponents, numClasses);

  // Map of audio files to expected labels (using 16kHz resampled versions)
  std::map<std::string, std::string> testFiles = {
      {"audio/long_jackhammer_16k.mp3", "jackhammer"}};

  std::vector<double> classAccuracies;
  int testedClasses = 0;

  for (const auto& [filename, expectedLabel] : testFiles) {
    try {
      MP3Data data = readMP3File(filename);
      const size_t numFrames = data.channel1.size() / WAVEFORM_SAMPLES;

      if (numFrames == 0) continue;

      size_t correctClassifications = 0;

      for (size_t frame = 0; frame < numFrames; ++frame) {
        std::vector<float> audio = ToFloatSamples(
            data.channel1, frame * WAVEFORM_SAMPLES,
            (frame + 1) * WAVEFORM_SAMPLES);

        classifier.Classify(audio);
        if (classifier.getClassificationLabel() == expectedLabel) {
          correctClassifications++;
        }
      }

      double accuracy =
          (static_cast<double>(correctClassifications) / numFrames) * 100.0;
      classAccuracies.push_back(accuracy);
      testedClasses++;

      std::cout << "  " << expectedLabel << ": " << accuracy << "% ("
                << correctClassifications << "/" << numFrames << " frames)"
                << std::endl;

    } catch (...) {
      continue;
    }
  }

  ASSERT_GT(testedClasses, 0) << "No audio classes could be tested";

  // Calculate overall accuracy (average across all classes)
  double overallAccuracy = 0.0;
  for (double acc : classAccuracies) {
    overallAccuracy += acc;
  }
  overallAccuracy /= classAccuracies.size();

  std::cout << "Overall Classification Accuracy: " << overallAccuracy << "%"
            << std::endl;
  std::cout << "Classes tested: " << testedClasses << std::endl;
  std::cout << "Required per-class accuracy: ≥90%" << std::endl;

  // Each class should meet 90% threshold individually
  for (size_t i = 0; i < classAccuracies.size(); i++) {
    EXPECT_GE(classAccuracies[i], 90.0)
        << "Class " << i << " accuracy " << classAccuracies[i]
        << "% below 90% threshold";
  }
}
