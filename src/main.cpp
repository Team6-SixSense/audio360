/**
 ******************************************************************************
 * @file    main.cpp
 * @brief   Main entry code for Audio360.
 ******************************************************************************
 */

#ifdef STM_BUILD
#include "hardware_interface/system/peripheral.h"
#else
#include "features/signal_processing/fft.h"
#endif

#include "helper/logging/logging.hpp"
#include <stdio.h>

int main() {

#ifdef STM_BUILD
  // Set-up pheripherals. Must call before any hardware function calls.
  setupPeripherals();
#else
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()));
#endif

  while (1) {
    INFO("Hello SixSense!");

    DEBUG("Audio360 is running.");
  }

  return 0;
}
