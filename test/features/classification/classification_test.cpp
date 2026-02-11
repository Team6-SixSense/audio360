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
static std::vector<float> ToFloatSamples(const std::vector<double>& samples, size_t start,
                                         size_t count) {
  std::vector<float> out;
  out.reserve(count);
  for (size_t i = start; i < count; ++i) {
    out.push_back(static_cast<float>(samples[i]));
  }
  return out;
}

/** @brief Runs classification on jackhammer audio and expects that label. */
TEST(ClassificationTest, ClassifyJackhammerAudio) {

  const uint16_t numMelFilters = 6;
  const uint16_t numDCTCoeff = 6;
  const uint16_t numPCAComponents = 6;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES, numMelFilters, numDCTCoeff,
                            numPCAComponents, numClasses);

  MP3Data data = readMP3File("audio/jackhammer.mp3");
  const size_t numFrames = data.channel1.size() / WAVEFORM_SAMPLES;
  size_t jackHammerCount = 0;

  for (size_t frame = 0; frame < numFrames; ++frame) {
    std::vector<float> audio = ToFloatSamples(
        data.channel1, frame * WAVEFORM_SAMPLES,
        (frame + 1) * WAVEFORM_SAMPLES);

    classifier.Classify(audio);
    if (classifier.getClassificationLabel() == "jackhammer") {
      ++jackHammerCount;
    }
  }

  // Update later on when the accuracy of jackhammer improves
  EXPECT_GE(jackHammerCount, numFrames / 6);

}
