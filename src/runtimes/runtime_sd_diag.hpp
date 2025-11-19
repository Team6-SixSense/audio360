/**
 ******************************************************************************
 * @file    runtime_sd_diag.hpp
 * @brief   Runtime SD card diagnostics code.
 ******************************************************************************
 */

#pragma once

#include "helper/constants.h"
#include "helper/logging/sd_writer.h"

void main_sd_diag() {
  SDCardWriter *sdcardWriterPointer1 = new SDCardWriter("mic1");
  SDCardWriter *sdcardWriterPointer2 = new SDCardWriter("mic2");
  SDCardWriter *sdcardWriterPointer3 = new SDCardWriter("mic3");
  SDCardWriter *sdcardWriterPointer4 = new SDCardWriter("mic4");

  int32_t waveform_buffer1[WAVEFORM_SAMPLES];
  int32_t waveform_buffer2[WAVEFORM_SAMPLES];
  int32_t waveform_buffer3[WAVEFORM_SAMPLES];
  int32_t waveform_buffer4[WAVEFORM_SAMPLES];

  SAI_HandleTypeDef *mic1 = getSAI1A_Handle();
  SAI_HandleTypeDef *mic2 = getSAI1B_Handle();
  SAI_HandleTypeDef *mic3 = getSAI2A_Handle();
  SAI_HandleTypeDef *mic4 = getSAI2B_Handle();

  while (1) {
    for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
      // TODO rename sample1 to something better.
      uint32_t sampleMic1 = 0;
      uint32_t sampleMic2 = 0;
      uint32_t sampleMic3 = 0;
      uint32_t sampleMic4 = 0;

      // Receive one sample.
      HAL_StatusTypeDef status =
          HAL_SAI_Receive(mic1, (uint8_t *)&sampleMic1, 1, 100);

      HAL_StatusTypeDef status2 =
          HAL_SAI_Receive(mic2, (uint8_t *)&sampleMic2, 1, 100);

      HAL_StatusTypeDef status3 =
          HAL_SAI_Receive(mic3, (uint8_t *)&sampleMic3, 1, 100);

      HAL_StatusTypeDef status4 =
          HAL_SAI_Receive(mic4, (uint8_t *)&sampleMic4, 1, 100);

      if (status == HAL_OK) {
        // Sign-extend the 24-bit sample to a 32-bit signed integer
        waveform_buffer1[i] = ((int32_t)(sampleMic1 & 0x00FFFFFF) << 8);
      } else {
        // If there's an error, just record a zero
        waveform_buffer1[i] = 0;
      }

      if (status2 == HAL_OK) {
        // Sign-extend the 24-bit sample to a 32-bit signed integer
        waveform_buffer2[i] = ((int32_t)(sampleMic2 & 0x00FFFFFF) << 8);
      } else {
        // If there's an error, just record a zero
        waveform_buffer2[i] = 0;
      }

      if (status3 == HAL_OK) {
        // Sign-extend the 24-bit sample to a 32-bit signed integer
        waveform_buffer3[i] = ((int32_t)(sampleMic3 & 0x00FFFFFF) << 8);
      } else {
        // If there's an error, just record a zero
        waveform_buffer3[i] = 0;
      }

      if (status4 == HAL_OK) {
        // Sign-extend the 24-bit sample to a 32-bit signed integer
        waveform_buffer4[i] = ((int32_t)(sampleMic4 & 0x00FFFFFF) << 8);
      } else {
        // If there's an error, just record a zero
        waveform_buffer4[i] = 0;
      }

      sdcardWriterPointer1->write_int32_buffer(waveform_buffer1,
                                               WAVEFORM_SAMPLES);

      sdcardWriterPointer2->write_int32_buffer(waveform_buffer2,
                                               WAVEFORM_SAMPLES);

      sdcardWriterPointer3->write_int32_buffer(waveform_buffer3,
                                               WAVEFORM_SAMPLES);

      sdcardWriterPointer4->write_int32_buffer(waveform_buffer4,
                                               WAVEFORM_SAMPLES);
    }
  }
}
