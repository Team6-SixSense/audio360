/**
******************************************************************************
* @file    bluetooth_manager.h
* @brief   Simple Bluetooth Manager that checks bluetooth state and transmits
* bytes.
******************************************************************************
*/
#ifndef AUDIO360_BLUETOOTH_MANAGER_H
#define AUDIO360_BLUETOOTH_MANAGER_H

#include <stdint.h>

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"


#ifdef __cplusplus
extern "C" {
#endif

enum Bluetooth_State { BLUETOOTH_IDLE, BLUETOOTH_CONNECTED, BLUETOOTH_ERROR };

/***
 * @brief Struct for storing a bluetooth connection state
 **/
typedef struct Bluetooth_Manager {
  uint8_t data_buffer[16];  // Small buffer for receiving strings

  volatile Bluetooth_State state = BLUETOOTH_IDLE;  // Bluetooth state.

  UART_HandleTypeDef* uart_handle;  // UART handle.
};

/** @brief Initializes bluetooth manager. */
void Bluetooth_Manager_Init();

/** @brief Checks bluetooth connection and update internal bluetooth state
 * management. */
void Bluetooth_Manager_Process();

/**
 * @brief Checks if bluetooth is connected.
 *
 * @return uint8_t It is 1 if bluetooth is connected
 */
uint8_t Is_Bluetooth_Connected();

/**
 * @brief Send bluetooth message.
 *
 * @param data Data bytestream.
 * @param numBytes The number of bytes in @ref data byetstream.
 * @return HAL_StatusTypeDef State of sending message via blueooth.
 */
HAL_StatusTypeDef Bluetooth_Manager_Send(uint8_t* data, uint16_t numBytes);

#ifdef __cplusplus
}
#endif

#endif  // AUDIO360_BLUETOOTH_MANAGER_H
