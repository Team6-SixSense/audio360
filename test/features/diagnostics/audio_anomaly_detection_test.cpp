/**
 ******************************************************************************
 * @file    audio_anomaly_detection_test.cpp
 * @brief   Unit tests for Audio Anomaly Detection module.
 ******************************************************************************
 */

#include "audio_anomaly_detection.h"

#include <gtest/gtest.h>

#include "constants.h"
#include "test_helper.h"

constexpr inline int AUDIO_STREAM_SIZE = 10;

/** @brief Test class for Audio Anomaly Detection class. */
class AudioAnomalyDetectionTest : public AudioAnomalyDectection,
                                  public ::testing::Test {
 public:
  /** @brief Construct Audio Anomaly Detectect test object. */
  AudioAnomalyDetectionTest() {}

  /** @brief Insert audio samples that display no audio anomaly behavior. */
  void createNormalAudio(int32_t* audioStream, int audioSize) {
    for (int i = 0; i < audioSize; i++) {
      audioStream[i] =
          generateRandomInt(MIN_AUDIO_SAMPLE_DATA, MAX_AUDIO_SAMPLE_DATA);
    }
  }

  /** @brief Insert audio samples that display clipper behavior. */
  void createClippingAudio(int32_t* audioStream, int audioSize) {
    for (int i = 0; i < audioSize; i++) {
      int choice = generateRandomInt(0, 2);

      if (choice == 0) {
        // No clipping.
        audioStream[i] =
            generateRandomInt(MIN_AUDIO_SAMPLE_DATA, MAX_AUDIO_SAMPLE_DATA);
      } else if (choice == 1) {
        // Clipping on negative side.
        audioStream[i] = MIN_AUDIO_SAMPLE_DATA - 1;
      } else {
        // Clipping on positive side.
        audioStream[i] = MAX_AUDIO_SAMPLE_DATA + 1;
      }
    }
  }

  /** @brief Insert audio samples that display lost signal behavior. */
  void createLostSignalAudio(int32_t* audioStream, int audioSize) {
    for (int i = 0; i < audioSize; i++) {
      audioStream[i] = 0;
    }
  }
};

/** @brief Given audio stream with no audio anomalies, Audio Anomaly Detection
 * reports that no audio anomalies are detected. */
TEST_F(AudioAnomalyDetectionTest, NoAudioAnomaly) {
  // Create audio streams with no audio anomalies.
  int32_t audioStream1[AUDIO_STREAM_SIZE];
  int32_t audioStream2[AUDIO_STREAM_SIZE];

  createNormalAudio(audioStream1, AUDIO_STREAM_SIZE);
  createNormalAudio(audioStream2, AUDIO_STREAM_SIZE);

  // Run audio anomaly detection on the audio streams.
  std::vector<int32_t*> audioStreams{audioStream1, audioStream2};
  bool anomalyDetected = checkAnomalies(audioStreams, AUDIO_STREAM_SIZE);

  // Assert no anomalies detected.
  ASSERT_FALSE(anomalyDetected);
}

/** @brief Given an audio stream with clipping anomalies, Audio Anomaly
 * Detection reports that audio anomalies are detected. */
TEST_F(AudioAnomalyDetectionTest, ClippingAnomaly) {
  // Create audio streams with clipping anomalies.
  int32_t audioStream1[AUDIO_STREAM_SIZE];
  int32_t audioStream2[AUDIO_STREAM_SIZE];

  createNormalAudio(audioStream1, AUDIO_STREAM_SIZE);
  createClippingAudio(audioStream2, AUDIO_STREAM_SIZE);

  // Run audio anomaly detection on the audio streams.
  std::vector<int32_t*> audioStreams{audioStream1, audioStream2};
  bool anomalyDetected = checkAnomalies(audioStreams, AUDIO_STREAM_SIZE);

  // Assert anomalies detected.
  ASSERT_TRUE(anomalyDetected);
}

/** @brief Given an audio stream with lost signal anomalies, Audio Anomaly
 * Detection reports that audio anomalies are detected. */
TEST_F(AudioAnomalyDetectionTest, LostSignalAnomaly) {
  // Create audio streams with clipping anomalies.
  int32_t audioStream1[AUDIO_STREAM_SIZE];
  int32_t audioStream2[AUDIO_STREAM_SIZE];

  createNormalAudio(audioStream1, AUDIO_STREAM_SIZE);
  createLostSignalAudio(audioStream2, AUDIO_STREAM_SIZE);

  // Run audio anomaly detection on the audio streams.
  std::vector<int32_t*> audioStreams{audioStream1, audioStream2};
  bool anomalyDetected = checkAnomalies(audioStreams, AUDIO_STREAM_SIZE);

  // Assert anomalies detected.
  ASSERT_TRUE(anomalyDetected);
}

/** @brief Given an audio stream with clipping and lost signal anomalies, Audio
 * Anomaly Detection reports that audio anomalies are detected. */
TEST_F(AudioAnomalyDetectionTest, ClippingAndLostSignalAnomaly) {
  // Create audio streams with clipping anomalies.
  int32_t audioStream1[AUDIO_STREAM_SIZE];
  int32_t audioStream2[AUDIO_STREAM_SIZE];

  createClippingAudio(audioStream1, AUDIO_STREAM_SIZE);
  createLostSignalAudio(audioStream2, AUDIO_STREAM_SIZE);

  // Run audio anomaly detection on the audio streams.
  std::vector<int32_t*> audioStreams{audioStream1, audioStream2};
  bool anomalyDetected = checkAnomalies(audioStreams, AUDIO_STREAM_SIZE);

  // Assert anomalies detected.
  ASSERT_TRUE(anomalyDetected);
}
