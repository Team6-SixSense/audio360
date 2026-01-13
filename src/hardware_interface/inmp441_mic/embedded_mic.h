//
// Created by Omar on 2025-10-16.
//

#ifndef EMBEDDED_MIC_H
#define EMBEDDED_MIC_H

#include "stm32f7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum embedded_mic_index {
  MIC_A1, MIC_A2, MIC_B1, MIC_B2
} embedded_mic_index;

typedef struct embedded_mic_t {
  embedded_mic_index index;
  SAI_HandleTypeDef hsai_block;
  IRQn_Type irq;
  DMA_HandleTypeDef* hdma_sai;
  int32_t* pBuffer;
  uint32_t BufferSize;
  volatile uint8_t half_rx_compl;
  volatile uint8_t full_rx_compl;
} embedded_mic_t;

void embedded_mic_init();

void embedded_mic_start(embedded_mic_t* mic_handle);

embedded_mic_t* embedded_mic_get(embedded_mic_index index);

#ifdef __cplusplus
}
#endif

#endif //EMBEDDED_MIC_H
