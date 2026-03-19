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

float DOA::calculateDirection(float* mic1Data, float* mic2Data, float* mic3Data,
                              float* mic4Data, DOA_Algorithms algo) {
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
