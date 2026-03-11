/**
 ******************************************************************************
 * @file    audio_anomaly_detection.cpp
 * @brief   Audio Anomaly Detection source.
 ******************************************************************************
 */

#include "audio_anomaly_detection.h"

#include "constants.h"

AudioAnomalyDectection::AudioAnomalyDectection() {}

bool AudioAnomalyDectection::checkAnomalies(std::vector<int32_t*> audioStreams,
                                            size_t audioStreamSize) {
  bool anomalyPresent = false;
  for (int32_t* audioStream : audioStreams) {
    bool lostSignal = true;

    // Check audio anomaly over each audio sample in the audio stream.
    for (int sample = 0; sample < audioStreamSize; sample++) {
      // Clipping detection.
      if (audioStream[sample] < MIN_AUDIO_SAMPLE_DATA ||
          audioStream[sample] > MAX_AUDIO_SAMPLE_DATA) {
        anomalyPresent = true;
        break;
      }

      // Lost signal detection.
      if (audioStream[sample] != 0) {
        lostSignal = false;
      }
    }

    anomalyPresent |= lostSignal;

    // Break early if audio anomaly is detected early.
    if (anomalyPresent) {
      break;
    }
  }

  return anomalyPresent;
}
