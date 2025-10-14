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

#include <stdio.h>

int main() {

#ifdef STM_BUILD
  setupPeripherals();
#else
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()));
#endif
  int i = 0;

  while (1) {
    printf("Hello SixSense\n\r");
    HAL_Delay(1000);
    i++;
  }

  return 0;
}
