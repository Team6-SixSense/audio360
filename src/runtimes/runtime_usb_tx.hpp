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

inline void main_usb_tx() {


  embedded_mic_t * mic_a1 = embedded_mic_get(MIC_A1);

  // 3. Start DMA (Non-blocking). The DMA will now fill waveform_buffer1 automatically.
  embedded_mic_start(mic_a1);



  while (1) {
    // The data collection now happens in the background.
    // You should process data inside HAL_SAI_RxCpltCallback / HAL_SAI_RxHalfCpltCallback
    // or set a flag there and process it here.

    // Note: The raw DMA data needs 'reorderMicData' applied during processing.
    uint8_t mic_a1_half = 0;
    uint8_t mic_a1_full = 0;

    __disable_irq();

    if (mic_a1->half_rx_compl) {
      mic_a1_half = 1;
      mic_a1->half_rx_compl = 0;
    }

    if (mic_a1->full_rx_compl) {
      mic_a1_full = 1;
      mic_a1->full_rx_compl = 0;
    }

    __enable_irq();

    int32_t* pDataToProcess = nullptr;

    uint32_t samplesProcess = WAVEFORM_SAMPLES / 2;

    if (mic_a1_half) {
      pDataToProcess = &mic_a1->pBuffer[0];
    }

    if (mic_a1_full) {
      pDataToProcess = &mic_a1->pBuffer[WAVEFORM_SAMPLES / 2];
    }

    if (pDataToProcess != nullptr) {
      for (uint32_t i = 0; i < samplesProcess; i++) {
        pDataToProcess[i] = reorderMicData(pDataToProcess[i]);
      }

      CDC_Transmit_FS((uint8_t*)pDataToProcess, samplesProcess * 4);
    }


  }
}
