/**
 ******************************************************************************
 * @file    runtime.hpp
 * @brief   FFT runtime code.
 ******************************************************************************
 */

#pragma once

#include "features/signal_processing/fft.h"
#include "helper/constants.h"

void main_runtime_fft() {
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()), SAMPLE_FREQUENCY);

  while (1) {
  }
}
