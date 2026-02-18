/**
 ******************************************************************************
 * @file    doaAlgorithm.h
 * @brief   Direction of Arrival (DoA) algorithms header.
 ******************************************************************************
 */

#pragma once

#include <vector>

/** @brief Enum representing the different DOA algorithms. */
enum DOA_Algorithms {
  NONE,
  GCC_PHAT,
};

/** @brief Abstract DOA algorithm class. */
class DoAAlgo {
 public:
  /**
   * @brief Calculate the direction (angle in radians) of audio source.
   *
   * @param mic1Data Audio data stream from microphone 1.
   * @param mic2Data Audio data stream from microphone 2.
   * @param mic3Data Audio data stream from microphone 3.
   * @param mic4Data Audio data stream from microphone 4.
   * @param algo DOA algorithm to use.
   * @return float Direction of audio source in radians.
   */
  virtual float calculateDirection(std::vector<float>& mic1Data,
                                   std::vector<float>& mic2Data,
                                   std::vector<float>& mic3Data,
                                   std::vector<float>& mic4Data) = 0;
};
