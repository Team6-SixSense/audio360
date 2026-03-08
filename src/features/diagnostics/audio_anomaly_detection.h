/**
 ******************************************************************************
 * @file    audio_anomaly_detection.h
 * @brief   Audio Anomaly Detection header.
 ******************************************************************************
 */

#pragma once

#include <cstdint>

#include "filter.hpp"
#include "system_fault_manager.h"

/** @brief Class responsible for detecting and reporting anomalies in audio
 * signals. */
class AudioAnomalyDectection {
 public:
  /** @brief Construct a new Audio Anomaly Detection object. */
  AudioAnomalyDectection();

  /**
   * @brief Check and report of any audio anomalies from input audio streams.
   *
   * @param audioStreams Input audio streams. All streams must be of the same
   * size defined in @ref audioStreamSize.
   * @param audioStreamSize The number of audio samples in a single audio
   * stream.
   * @return True if audio anomalies exists, otherwise returns false.
   */
  bool checkAnomalies(std::vector<int32_t*> audioStreams,
                      size_t audioStreamSize);
};
