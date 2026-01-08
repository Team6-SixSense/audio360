/**
 ******************************************************************************
 * @file    runtime_usb_tx.hpp
 * @brief   Runtime USART TX code.
 ******************************************************************************
 */

#pragma once

#include "bit_operations.hpp"
#include "constants.h"
#include "peripheral.h"
#include "usbd_cdc_if.h"
#include "embedded_mic.h"

void mainUSB_TX() {
  int32_t waveform_buffer1[WAVEFORM_SAMPLES];

  SAI_HandleTypeDef* mic1 = &embedded_mic_get(MIC_A1)->hsai_block;

  while (1) {
    for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
      uint32_t sampleMic1 = 0;

      // Receive one sample.
      HAL_StatusTypeDef status =
          HAL_SAI_Receive(mic1, (uint8_t*)&sampleMic1, 1, 100);

      if (status == HAL_OK) {
        // Re-order mic data so it can be interpreted correctly.
        waveform_buffer1[i] = reorderMicData(sampleMic1);
      } else {
        // If there's an error, just record a zero
        waveform_buffer1[i] = 0;
      }
    }

    CDC_Transmit_FS((uint8_t*)waveform_buffer1, WAVEFORM_SAMPLES * 4);
  }
}
