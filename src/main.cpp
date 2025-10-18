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

#define DEBUG_I2S_MIC

#include "helper/logging/logging.hpp"
#include <stdio.h>

// Define the number of samples you want to capture for one waveform snapshot
#define WAVEFORM_SAMPLES 256

int main() {

#ifdef STM_BUILD
  // Set-up pheripherals. Must call before any hardware function calls.
  setupPeripherals();
#else
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()));
#endif

  while (1) {

#ifdef DEBUG_I2S_MIC
    int32_t waveform_buffer[WAVEFORM_SAMPLES];
    SAI_HandleTypeDef *handle = getSAI_Handle();

    // 1. Capture a buffer of audio samples
    for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
      uint32_t raw_sample = 0;
      // Receive one sample (based on your finding that size=1 works)
      HAL_StatusTypeDef status = HAL_SAI_Receive(handle, (uint8_t *)&raw_sample, 1, 100);

      if (status == HAL_OK) {
        // Sign-extend the 24-bit sample to a 32-bit signed integer
        if (raw_sample & 0x00800000) {
          waveform_buffer[i] = raw_sample | 0xFF000000;
        } else {
          waveform_buffer[i] = raw_sample;
        }
      } else {
        // If there's an error, just record a zero
        waveform_buffer[i] = 0;
      }
    }

    // 2. Print the captured waveform data to the serial console
    // Use standard printf for easy copy-pasting
    printf("---START_WAVEFORM_DATA---\r\n");
    for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
      printf("%ld\r\n", waveform_buffer[i]);
    }
    printf("---END_WAVEFORM_DATA---\r\n");

    // Delay for a couple of seconds before capturing the next waveform
    HAL_Delay(2000);
#endif

  }

  return 0;
}
