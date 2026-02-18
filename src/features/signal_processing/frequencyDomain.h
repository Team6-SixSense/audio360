/**
 ******************************************************************************
 * @file    frequencyDomain.h
 * @brief   Frequency domain header
 ******************************************************************************
 */

#pragma once

#include <cstdint>
#include <vector>

/** @brief Struct for representing FFT output in the frequency domain. */
struct FrequencyDomain {
  /** @brief The number of the data in the FFT output. */
  uint16_t N;

  /** @brief The frequency (Hz) */
  std::vector<float> frequency;

  /** @brief The real component of the frequency contribution. */
  std::vector<float> real;

  /** @brief The imaginary component of the frequency contribution. */
  std::vector<float> img;

  /** @brief The magnitude of the frequency component. */
  std::vector<float> magnitude;

  /** @brief The magnitude squared of the frequency component. */
  std::vector<float> powerMagnitude;

  /**
   * @brief Construct a new Frequency Domain struct.
   *
   * @param size The number of data points.
   */
  FrequencyDomain(uint16_t size)
      : N(size),
        frequency(size),
        real(size),
        img(size),
        magnitude(size),
        powerMagnitude(size) {}
};
