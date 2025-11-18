/**
 ******************************************************************************
 * @file    main.cpp
 * @brief   Main entry code for Audio360.
 ******************************************************************************
 */

#ifdef STM_BUILD
#include "hardware_interface/system/peripheral.h"
#include "helper/logging/sd_writer.h"
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

  SDCardWriter *sdcardWriterPointer = new SDCardWriter("audio");


  while (1) {

#ifdef DEBUG_I2S_MIC
    int32_t waveform_buffer1[WAVEFORM_SAMPLES];
    int32_t waveform_buffer2[WAVEFORM_SAMPLES];
    int32_t waveform_buffer3[WAVEFORM_SAMPLES];
    int32_t waveform_buffer4[WAVEFORM_SAMPLES];

    SAI_HandleTypeDef *mic1 = getSAI1A_Handle();
    SAI_HandleTypeDef *mic2 = getSAI1B_Handle();
    SAI_HandleTypeDef *mic3 = getSAI2A_Handle();
    SAI_HandleTypeDef *mic4 = getSAI2B_Handle();

    // 1. Capture a buffer of audio samples
    for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
      // TODO rename sample1 to something better.
      uint32_t sampleMic1 = 0;
      uint32_t sampleMic2 = 0;
      uint32_t sampleMic3 = 0;
      uint32_t sampleMic4 = 0;

      // Receive one sample.
      HAL_StatusTypeDef status =
          HAL_SAI_Receive(mic1, (uint8_t *)&sampleMic1, 1, 100);
      //
      // HAL_StatusTypeDef status2 =
      //     HAL_SAI_Receive(mic2, (uint8_t *)&sampleMic2, 1, 100);
      //
      // HAL_StatusTypeDef status3 =
      //     HAL_SAI_Receive(mic3, (uint8_t *)&sampleMic3, 1, 100);
      //
      // HAL_StatusTypeDef status4 =
      //     HAL_SAI_Receive(mic4, (uint8_t *)&sampleMic4, 1, 100);

      if (status == HAL_OK) {
        // Sign-extend the 24-bit sample to a 32-bit signed integer
        if (sampleMic1 & 0x00800000) {
          waveform_buffer1[i] = sampleMic1 | 0xFF000000;
        } else {
          waveform_buffer1[i] = sampleMic1;
        }
      } else {
        // If there's an error, just record a zero
        waveform_buffer1[i] = 0;
      }

      // if (status2 == HAL_OK) {
      //   // Sign-extend the 24-bit sample to a 32-bit signed integer
      //   if (sampleMic2 & 0x00800000) {
      //     waveform_buffer2[i] = sampleMic2 | 0xFF000000;
      //   } else {
      //     waveform_buffer2[i] = sampleMic2;
      //   }
      // } else {
      //   // If there's an error, just record a zero
      //   waveform_buffer2[i] = 0;
      // }
      //
      // if (status3 == HAL_OK) {
      //   // Sign-extend the 24-bit sample to a 32-bit signed integer
      //   if (sampleMic3 & 0x00800000) {
      //     waveform_buffer3[i] = sampleMic3 | 0xFF000000;
      //   } else {
      //     waveform_buffer3[i] = sampleMic3;
      //   }
      // } else {
      //   // If there's an error, just record a zero
      //   waveform_buffer3[i] = 0;
      // }
      //
      // if (status4 == HAL_OK) {
      //   // Sign-extend the 24-bit sample to a 32-bit signed integer
      //   if (sampleMic4 & 0x00800000) {
      //     waveform_buffer4[i] = sampleMic4 | 0xFF000000;
      //   } else {
      //     waveform_buffer4[i] = sampleMic4;
      //   }
      // } else {
      //   // If there's an error, just record a zero
      //   waveform_buffer4[i] = 0;
      // }
    }

    // 2. Print the captured waveform data to the serial console
    // Use standard printf for easy copy-pasting
    printf("---START_WAVEFORM_DATA 1---\r\n");
    std::string waveform_str = "";
    // for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
    //   //printf("%ld\r\n", waveform_buffer1[i]);
    //   waveform_str += std::to_string(waveform_buffer1[i])+",";
    //
    //   if (waveform_str.length() * sizeof(char) > 235) {
    //     sdcardWriterPointer->write(waveform_str.c_str());
    //     waveform_str = "";
    //   }
    //
    // }
    //
    // sdcardWriterPointer->write(waveform_str.c_str());
    sdcardWriterPointer->write_int32_buffer(waveform_buffer1, WAVEFORM_SAMPLES);
    printf("---END_WAVEFORM_DATA 1---\r\n");

    // printf("---START_WAVEFORM_DATA 2---\r\n");
    // for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
    //   printf("%ld\r\n", waveform_buffer2[i]);
    // }
    // printf("---END_WAVEFORM_DATA 2---\r\n");
    //
    // printf("---START_WAVEFORM_DATA 3---\r\n");
    // for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
    //   printf("%ld\r\n", waveform_buffer3[i]);
    // }
    // printf("---END_WAVEFORM_DATA 3---\r\n");
    //
    // printf("---START_WAVEFORM_DATA 4---\r\n");
    // for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
    //   printf("%ld\r\n", waveform_buffer4[i]);
    // }
    //printf("---END_WAVEFORM_DATA 4---\r\n");

    // Delay for a couple of seconds before capturing the next waveform

#endif
  }

  return 0;
}
