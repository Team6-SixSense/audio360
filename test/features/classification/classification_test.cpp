/**
 ******************************************************************************
 * @file    classification_test.cpp
 * @brief   Unit tests for the Classification pipeline on real audio input.
 ******************************************************************************
 */

#include "classification.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "constants.h"
#include "mp3.h"

/** @brief Converts MP3 channel samples to float samples for classification. */
static std::vector<float> ToFloatSamples(const std::vector<double>& samples,
                                         size_t count) {
  std::vector<float> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    out.push_back(static_cast<float>(samples[i]));
  }
  return out;
}

/** @brief Runs classification on jackhammer audio and expects that label. */
TEST(ClassificationTest, ClassifyJackhammerAudio) {
  MP3Data data = readMP3File("audio/jackhammer.mp3");
  const size_t requiredSamples = 4 * static_cast<size_t>(WAVEFORM_SAMPLES);

  ASSERT_GE(data.channel1.size(), requiredSamples);

  std::vector<float> audio = ToFloatSamples(data.channel1, requiredSamples);

  const uint16_t numMelFilters = 40;
  const uint16_t numDCTCoeff = 13;
  const uint16_t numPCAComponents = 13;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES, numMelFilters, numDCTCoeff,
                            numPCAComponents, numClasses);
  classifier.Classify(audio);
}
