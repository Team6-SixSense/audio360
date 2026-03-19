/**
 ******************************************************************************
 * @file    frequencyDomain.h
 * @brief   Frequency domain header
 ******************************************************************************
 */

#pragma once

#include <cstdint>

#include "constants.h"

/** @brief Struct for representing FFT output in the frequency domain. */
struct FrequencyDomain {
  /** @brief The number of the data in the FFT output. */
  uint16_t N = FREQ_DOMAIN_SIZE;

  /** @brief The frequency (Hz) */
  float frequency[FREQ_DOMAIN_SIZE];

  /** @brief The real component of the frequency contribution. */
  float real[FREQ_DOMAIN_SIZE];

  /** @brief The imaginary component of the frequency contribution. */
  float img[FREQ_DOMAIN_SIZE];

  /** @brief The magnitude of the frequency component. */
  float magnitude[FREQ_DOMAIN_SIZE];

  /** @brief The magnitude squared of the frequency component. */
  float powerMagnitude[FREQ_DOMAIN_SIZE];
};
