/**
 ******************************************************************************
 * @file    doa.h
 * @brief   Direction of Arrival (DoA) header.
 ******************************************************************************
 */

#pragma once

#include <vector>

#include "doaAlgorithm.h"
#include "gccPhat.h"

/** @brief DOA processing module. */
class DOA {
 public:
  /**
   * @brief Construct a new DOA object.
   *
   * @param numSamples Number of samples from each source that needs to be
   * processed.
   */
  DOA(size_t numSamples);

  /**
   * @brief Calculate the direction of audio source.
   *
   * @param mic1Data Audio data stream from microphone 1.
   * @param mic2Data Audio data stream from microphone 2.
   * @param mic3Data Audio data stream from microphone 3.
   * @param mic4Data Audio data stream from microphone 4.
   * @param algo DOA algorithm to use.
   * @return float Direction of audio source in radians.
   * @throws AudioProcessingException if failure in processing audio data.
   */
  float calculateDirection(std::vector<float>& mic1Data,
                           std::vector<float>& mic2Data,
                           std::vector<float>& mic3Data,
                           std::vector<float>& mic4Data,
                           DOA_Algorithms algo = DOA_Algorithms::GCC_PHAT);

 private:
  /**
   * @brief Checks that input audio data have the correct dimensions.
   *
   * @param mic1Data Audio data stream from microphone 1.
   * @param mic2Data Audio data stream from microphone 2.
   * @param mic3Data Audio data stream from microphone 3.
   * @param mic4Data Audio data stream from microphone 4.
   * @return true if all input have correct dimensions. False otherwise.
   */
  bool checkInputSize(const std::vector<float>& mic1Data,
                      const std::vector<float>& mic2Data,
                      const std::vector<float>& mic3Data,
                      const std::vector<float>& mic4Data);

  /** @brief The number of samples to process for each incoming source. */
  size_t numSamples;

  /** @brief GCC PhaT algorithm module. */
  GCCPhaT gccPhaT;
};
