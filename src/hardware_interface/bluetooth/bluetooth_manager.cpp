/**
******************************************************************************
* @file    bluetooth_manager.cpp
* @brief   Simple Bluetooth Manager that checks bluetooth state and transmits
* bytes. Implementation for bluetooth_manager.h
******************************************************************************
*/

#include "bluetooth_manager.h"

#include <cstdio>
#include <cstring>

static char* cmd = "AT\r\n";
static char* cmd_mac_addr = "AT+UUID?\r\n";

static Bluetooth_Manager bluetooth_manager;
extern UART_HandleTypeDef huart5;

void Bluetooth_Manager_Init() {
  bluetooth_manager.state = BLUETOOTH_IDLE;
  memset(bluetooth_manager.data_buffer, 0,
         sizeof(bluetooth_manager.data_buffer));
  bluetooth_manager.uart_handle = &huart5;
}

void Bluetooth_Manager_Process() {
  GPIO_PinState connected = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);

  if (connected == GPIO_PIN_SET) {
    bluetooth_manager.state = BLUETOOTH_CONNECTED;
  } else {
    bluetooth_manager.state = BLUETOOTH_IDLE;
  }

  if (bluetooth_manager.state == BLUETOOTH_IDLE) {
    // Send exactly the length of the string (4 bytes: A, T, \r, \n)
    HAL_UART_Transmit(bluetooth_manager.uart_handle, (uint8_t*)cmd, strlen(cmd),
                      100);

    // Clear buffer
    memset(bluetooth_manager.data_buffer, 0,
           sizeof(bluetooth_manager.data_buffer));

    // 4 bytes for "OK\r\n")
    HAL_UART_Receive(bluetooth_manager.uart_handle,
                     bluetooth_manager.data_buffer, 4, 1000);
    uint8_t* buffer = bluetooth_manager.data_buffer;
    if (!(buffer[0] == 'O' && buffer[1] == 'K' && buffer[2] == '\r' &&
          buffer[3] == '\n')) {
      bluetooth_manager.state = BLUETOOTH_ERROR;  // something wrong happened.
    }

    HAL_Delay(50);
  }
}

uint8_t Is_Bluetooth_Connected() {
#ifdef BUILD_BLUETOOTH
  return bluetooth_manager.state == BLUETOOTH_CONNECTED;
#else
  return 1U;  // Mocks bluetooth connected state.
#endif
}

HAL_StatusTypeDef Bluetooth_Manager_Send(uint8_t* data, uint16_t numBytes) {
#ifdef BUILD_BLUETOOTH
  return HAL_UART_Transmit(bluetooth_manager.uart_handle, data, numBytes, 100);
#else
  return HAL_OK;
#endif
}