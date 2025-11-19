/**
 ******************************************************************************
 * @file    runtime_usb_tx.hpp
 * @brief   Runtime USART TX code.
 ******************************************************************************
 */

#pragma once

#include "hardware_interface/system/peripheral.h"
#include "helper/constants.h"
#include "usbd_cdc_if.h"

void main_usb_tx() {
  int32_t waveform_buffer1[WAVEFORM_SAMPLES];

  SAI_HandleTypeDef *mic1 = getSAI1A_Handle();

  while (1) {
    for (int i = 0; i < WAVEFORM_SAMPLES; i++) {

      uint32_t sampleMic1 = 0;

      // Receive one sample.
      HAL_StatusTypeDef status =
          HAL_SAI_Receive(mic1, (uint8_t *)&sampleMic1, 1, 100);

      if (status == HAL_OK) {
        // Sign-extend the 24-bit sample to a 32-bit signed integer
        waveform_buffer1[i] = ((int32_t)(sampleMic1 & 0x00FFFFFF) << 8);
      } else {
        // If there's an error, just record a zero
        waveform_buffer1[i] = 0;
      }
    }

    CDC_Transmit_FS((uint8_t *)waveform_buffer1, WAVEFORM_SAMPLES * 4);
  }
}
