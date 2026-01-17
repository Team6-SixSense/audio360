/**
******************************************************************************
* @file    embedded_mic.h
* @brief   This file defines the structs and functions used to
* define an embedded INMP441/ICSC 442 microphone.
* @author  Omar Alam
******************************************************************************
*/

#ifndef EMBEDDED_MIC_H
#define EMBEDDED_MIC_H

#include "stm32f7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Embedded mic ID enum
 * This enum provides readable identifiers for each microphone
 * expected to be present in the system. The convention is as follows:
 * MIC_{SAI_BLOCK}{SAI_NUMBER}
 */
typedef enum embedded_mic_index {
  MIC_A1,
  MIC_A2,
  MIC_B1,
  MIC_B2
} embedded_mic_index;

/**
 * @brief Struct to store microphone details, buffered data and
 * flags.
 */
typedef struct embedded_mic_t {
  /** @brief  Embedded mic identifier from embedded_mic_index enum. */
  embedded_mic_index index;

  /** @brief Handle to the serial audio interface block. */
  SAI_HandleTypeDef hsai_block;

  /** @brief Interrupt request number. */
  IRQn_Type irq;

  /** @brief Handle for DMA channel/stream associated with this mic. */
  DMA_HandleTypeDef* hdma_sai;

  /** @brief Buffer to store streamed microphone data.  */
  int32_t* pBuffer;

  /** @brief  Size of the buffer in samples. */
  uint32_t BufferSize;

  /** @brief if DMA is finished writing to the first half of the buffer */
  volatile uint8_t half_rx_compl;

  /** @brief if DMA is finished writing to the second half of the buffer */
  volatile uint8_t full_rx_compl;
} embedded_mic_t;

/**
 * @brief Initialize all four microphones. This includes
 * SAI setup, synchronization setup and DMA setup.
 * @return None
 */
void embedded_mic_init();

/**
 * @brief Start the streaming DMA process for a specific microphone.
 * Once this function is called, streaming will be activated for this
 * microphone.
 * @param mic_handle pointer to struct for embedded mic.
 * @return None
 */
void embedded_mic_start(embedded_mic_t* mic_handle);

/**
 * @brief Retrieve pointer handle for a specific microphone
 * @param index ID of the microphone to retrieve pointer for
 * @return Pointer to instance of microphone requested
 */
embedded_mic_t* embedded_mic_get(embedded_mic_index index);

#ifdef __cplusplus
}
#endif

#endif  // EMBEDDED_MIC_H
