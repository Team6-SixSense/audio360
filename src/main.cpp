/**
 ******************************************************************************
 * @file    main.cpp
 * @brief   Main entry code for Audio360.
 ******************************************************************************
 */

#ifdef STM_BUILD
#include "peripheral_error.hpp"
#endif

#include <stdio.h>

#ifdef STM_BUILD
#define RUNTIME_AUDIO360
#else
#define RUNTIME_FFT
#endif

#ifdef STM_BUILD
#include "runtimes/runtime_android_comm.hpp"
#include "runtimes/runtime_audio360.hpp"
#include "runtimes/runtime_usb_tx.hpp"
#else
#include "runtimes/runtime_fft.hpp"
#endif

#ifdef BUILD_GLASSES_HOST
#define USBH_USE_OS 0
#include "usb_host.h"
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
  main_usb_tx();
#endif

#ifdef RUNTIME_ANDROID_COMM
  mainAndroidComm();
#endif

#ifdef RUNTIME_FFT
  mainRuntimeFFT();
#endif

  return 0;
}
