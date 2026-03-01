/**
 ******************************************************************************
 * @file    doa.cpp
 * @brief   Direction of Arrival (DoA) source.
 ******************************************************************************
 */

#include "doa.h"

#include <stdexcept>

#include "exceptions.hpp"
#include "logging.hpp"

DOA::DOA(size_t numSamples) : numSamples(numSamples), gccPhaT(numSamples) {}

float DOA::calculateDirection(std::vector<float>& mic1Data,
                              std::vector<float>& mic2Data,
                              std::vector<float>& mic3Data,
                              std::vector<float>& mic4Data,
                              DOA_Algorithms algo) {
  if (!this->checkInputSize(mic1Data, mic2Data, mic3Data, mic4Data)) {
    ERROR("Incorrect mic audio input data.");
    throw AudioProcessingException("Incorrect mic audio input data.");
  }

  float angle_rad = 0.0;

  switch (algo) {
    case GCC_PHAT:
      angle_rad =
          gccPhaT.calculateDirection(mic1Data, mic2Data, mic3Data, mic4Data);
      break;

    default:
      ERROR("DOA algorithm is currently not supported.");
      throw AudioProcessingException(
          "DOA algorithm is currently not supported.");
      break;
  }

  return angle_rad;
}

bool DOA::checkInputSize(const std::vector<float>& mic1Data,
                         const std::vector<float>& mic2Data,
                         const std::vector<float>& mic3Data,
                         const std::vector<float>& mic4Data) {
  if (mic1Data.size() == numSamples && mic2Data.size() == numSamples &&
      mic3Data.size() == numSamples && mic4Data.size() == numSamples) {
    return true;
  }

  return false;
}
