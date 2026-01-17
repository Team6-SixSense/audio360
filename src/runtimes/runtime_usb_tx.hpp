/**
 ******************************************************************************
 * @file    runtime_usb_tx.hpp
 * @brief   Runtime USART TX code.
 ******************************************************************************
 */

#pragma once

#include "bit_operations.hpp"
#include "constants.h"
#include "embedded_mic.h"
#include "peripheral.h"
#include "usbd_cdc_if.h"

static int32_t debug_buffer[WAVEFORM_SAMPLES * 4];

inline void main_usb_tx() {
  embedded_mic_t* mic_a1 = embedded_mic_get(MIC_A1);
  embedded_mic_t* mic_a2 = embedded_mic_get(MIC_A2);
  embedded_mic_t* mic_b1 = embedded_mic_get(MIC_B1);
  embedded_mic_t* mic_b2 = embedded_mic_get(MIC_B2);

  // Start DMA (Non-blocking).
  embedded_mic_start(mic_a2);
  embedded_mic_start(mic_b1);
  embedded_mic_start(mic_b2);
  embedded_mic_start(mic_a1);

  while (1) {
    // The data collection now happens in the background.

    // Note: The raw DMA data needs 'reorderMicData' applied during processing.
    // We can just check the flag for one of the mics and it should be the same
    // as all the other mics.

    uint8_t mic_a1_half = 0;
    uint8_t mic_a1_full = 0;

    uint8_t dummy_half = 0;
    uint8_t dummy_full = 0;

    // we clear states for all of them though
    check_mic_buffers(mic_a1, &mic_a1_half, &mic_a1_full);
    check_mic_buffers(mic_a2, &dummy_half, &dummy_full);
    check_mic_buffers(mic_b1, &dummy_half, &dummy_full);
    check_mic_buffers(mic_b2, &dummy_half, &dummy_full);

    uint32_t samplesProcess = WAVEFORM_SAMPLES / 2;

    // Process states sequentially to avoid dropping frames if both flags are
    // set
    for (int step = 0; step < 2; step++) {
      bool process = false;
      uint32_t offset = 0;

      if (step == 0 && mic_a1_half) {
        process = true;
        offset = 0;
      } else if (step == 1 && mic_a1_full) {
        process = true;
        offset = WAVEFORM_SAMPLES / 2;
      }

      if (process) {
        int32_t* srcA1 = &mic_a1->pBuffer[offset];
        int32_t* srcA2 = &mic_a2->pBuffer[offset];
        int32_t* srcB1 = &mic_b1->pBuffer[offset];
        int32_t* srcB2 = &mic_b2->pBuffer[offset];

        // 1. Invalidate Source Cache (CPU reads from RAM updated by DMA)
        SCB_InvalidateDCache_by_Addr((uint32_t*)srcA1, samplesProcess * 4);
        SCB_InvalidateDCache_by_Addr((uint32_t*)srcA2, samplesProcess * 4);
        SCB_InvalidateDCache_by_Addr((uint32_t*)srcB1, samplesProcess * 4);
        SCB_InvalidateDCache_by_Addr((uint32_t*)srcB2, samplesProcess * 4);

        // 2. Interleave Data
        for (uint32_t i = 0; i < samplesProcess; i++) {
          debug_buffer[i * 4 + 0] = reorderMicData(srcA1[i]);  // A1
          debug_buffer[i * 4 + 1] = reorderMicData(srcB1[i]);  // B1
          debug_buffer[i * 4 + 2] = reorderMicData(srcA2[i]);  // A2
          debug_buffer[i * 4 + 3] = reorderMicData(srcB2[i]);  // B2
        }

        // 3. Clean Destination Cache (USB DMA reads from RAM updated by CPU)
        // Ensure the data we just wrote to debug_buffer is flushed from Cache
        // to RAM
        uint32_t total_bytes = samplesProcess * 4 * 4;
        SCB_CleanDCache_by_Addr((uint32_t*)debug_buffer, total_bytes);

        // 4. Transmit via USB with robust retry
        uint8_t* tx_ptr = (uint8_t*)debug_buffer;
        const uint32_t chunk_size = 4096;
        uint32_t sent_bytes = 0;
        const uint32_t max_retries = 2000000;  // Increased timeout (~20-50ms)

        while (sent_bytes < total_bytes) {
          uint32_t len = (total_bytes - sent_bytes > chunk_size)
                             ? chunk_size
                             : (total_bytes - sent_bytes);
          uint8_t status;
          uint32_t retry_count = 0;

          do {
            status = CDC_Transmit_FS(tx_ptr + sent_bytes, len);
            retry_count++;
          } while (status == USBD_BUSY && retry_count < max_retries);

          if (status == USBD_OK) {
            sent_bytes += len;
          } else {
            // If we timeout, we break. This will cause a glitch, but we tried
            // our best.
            break;
          }
        }
      }
    }
  }
}
