/**
 ******************************************************************************
 * @file    main.cpp
 * @brief   Main entry code for Audio360.
 ******************************************************************************
 */

#ifdef STM_BUILD
#include "peripheral.h"
#endif

#include <stdio.h>

#ifdef STM_BUILD
#define RUNTIME_AUDIO360
#else
#define RUNTIME_FFT
#endif

#if defined(STM_BUILD) && defined(RUNTIME_FFT)
#error "The SimpleFFT runtime is not meant for the STM32 MCU".
#endif

#ifdef STM_BUILD
#include "runtimes/runtime_audio360.hpp"
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

#ifdef RUNTIME_AUDIO360
  mainAudio360();
#endif

#ifdef RUNTIME_USB_TX
  mainUSB_TX();
#endif

#ifdef RUNTIME_SD_DIAG
  mainSDDiag();
#endif

#ifdef RUNTIME_FFT
  mainRuntimeFFT();
#endif

  return 0;
}
