/**
 ******************************************************************************
 * @file    main.cpp
 * @brief   Main entry code for Audio360.
 ******************************************************************************
 */

#ifdef STM_BUILD
#include "hardware_interface/system/peripheral.h"
#endif

#include <stdio.h>

#define RUNTIME_USB_TX

#if defined(STM_BUILD) && defined(RUNTIME_FFT)
#error "The SimpleFFT runtime is not meant for the STM32 MCU".
#endif

#ifdef STM_BUILD
#include "runtimes/runtime_sd_diag.hpp"
#include "runtimes/runtime_usb_tx.hpp"
#else
#include "runtimes/runtime_fft.hpp"
#endif

int main() {

#ifdef STM_BUILD
  // Set-up peripherals. Must call before any hardware function calls.
  setupPeripherals();
#endif

#ifdef RUNTIME_USB_TX
  main_usb_tx();
#endif

#ifdef RUNTIME_SD_DIAG
  main_sd_diag();
#endif

#ifdef RUNTIME_FFT
  main_runtime_fft();
#endif

  return 0;
}
