/**
 ******************************************************************************
 * @file    runtime_fft.hpp
 * @brief   FFT runtime code.
 ******************************************************************************
 */

#pragma once

#include "constants.h"
#include "fft.h"

void mainRuntimeFFT() {
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()), SAMPLE_FREQUENCY);

  while (1) {
  }
}
