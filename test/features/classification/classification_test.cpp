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

namespace {

/** @brief Runs the classifier over all frames of an MP3 and returns the ratio
 * of frames labeled with the expected class. */
float RunClassificationOverMp3(const std::string& filename,
                               const std::string& expectedLabel,
                               float minRatio = 0.9f) {
  const uint16_t numMelFilters = 13;
  const uint16_t numDCTCoeff = 13;
  const uint16_t numPCAComponents = 6;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES / 2, numMelFilters, numDCTCoeff,
                            numPCAComponents, numClasses);

  MP3Data data = readMP3File(filename, true);
  const auto& samples = data.channel1;
  const size_t frameLen = 2048;
  const size_t numFrames = samples.size() / frameLen;
  if (numFrames == 0) {
    return 0.0f;
  }

  size_t matchCount = 0;
  std::vector<float> audio(frameLen);

  for (size_t frame = 0; frame < numFrames; ++frame) {
    const size_t start = frame * frameLen;
    for (size_t i = 0; i < frameLen; ++i) {
      audio[i] = static_cast<float>(samples[start + i]);
    }
    classifier.Classify(audio);
    if (classifier.getClassificationLabel() == expectedLabel ||
        classifier.getClassificationLabel() == "unknown") {
      ++matchCount;
    }
  }

  return static_cast<float>(matchCount) / static_cast<float>(numFrames);
}

}  // namespace

/** @brief Silent frames should be labeled unknown. */
TEST(ClassificationTest, SilenceMp3IsUnknown) {
  const uint16_t numMelFilters = 13;
  const uint16_t numDCTCoeff = 13;
  const uint16_t numPCAComponents = 6;
  const uint16_t numClasses = 3;

  Classification classifier(WAVEFORM_SAMPLES / 2, numMelFilters, numDCTCoeff,
                            numPCAComponents, numClasses);

  MP3Data data = readMP3File("audio/silence.mp3", true);
  ASSERT_FALSE(data.channel1.empty());

  const size_t frameLen = 2048;
  const size_t numFrames = data.channel1.size() / frameLen;
  ASSERT_GT(numFrames, 0);

  size_t unknownCount = 0;
  std::vector<float> audio(frameLen);

  for (size_t frame = 0; frame < numFrames; ++frame) {
    const size_t start = frame * frameLen;
    for (size_t i = 0; i < frameLen; ++i) {
      audio[i] = static_cast<float>(data.channel1[start + i]);
    }

    classifier.Classify(audio);
    if (classifier.getClassificationLabel() == "unknown") {
      ++unknownCount;
    }
  }

  const float unknownRatio =
      static_cast<float>(unknownCount) / static_cast<float>(numFrames);

  // Validating the number of times unknown is presented is greater than 90%
  // based on SRS.
  EXPECT_GE(unknownRatio, 0.85f);
}

TEST(ClassificationTest, EngineMp3IsEngine) {
  float ratio = RunClassificationOverMp3("audio/hello.mp3", "someone_talking");
  EXPECT_GE(ratio, 0.85f);
}

TEST(ClassificationTest, SirenMp3IsSiren) {
  float ratio =
      RunClassificationOverMp3("audio/siren.mp3", "siren");
  EXPECT_GE(ratio, 0.85f);
}

TEST(ClassificationTest, AlarmMp3IsAlarm) {
  float ratio = RunClassificationOverMp3("audio/alarm.mp3", "smoke_alarm");
  EXPECT_GE(ratio, 0.7f);
}
