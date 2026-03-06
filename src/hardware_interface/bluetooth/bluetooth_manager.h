/**
******************************************************************************
* @file    bluetooth_manager.h
* @brief   Simple Bluetooth Manager that checks bluetooth state and transmits
* bytes.
******************************************************************************
*/
#ifndef AUDIO360_BLUETOOTH_MANAGER_H
#define AUDIO360_BLUETOOTH_MANAGER_H

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

enum Bluetooth_State {
  BLUETOOTH_IDLE,
  BLUETOOTH_CONNECTED,
  BLUETOOTH_ERROR
};

/***
 * @brief Struct for storing a bluetooth connection state
 **/
typedef struct Bluetooth_Manager {
  uint8_t data_buffer[16]; // Small buffer for receiving strings
  volatile Bluetooth_State state = BLUETOOTH_IDLE;
  UART_HandleTypeDef* uart_handle;
};

void Bluetooth_Manager_Init();

void Bluetooth_Manager_Process();

HAL_StatusTypeDef Bluetooth_Manager_Send(uint8_t *data);

uint8_t Is_Bluetooth_Connected();

#ifdef __cplusplus
  }
#endif

#endif  // AUDIO360_BLUETOOTH_MANAGER_H
