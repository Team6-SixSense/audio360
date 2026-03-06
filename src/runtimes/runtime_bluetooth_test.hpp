/**
******************************************************************************
* @file    runtime_bluetooth_test.hpp
* @brief   Runtime code for testing bluetooth communication.
******************************************************************************
*/
#pragma once

#include "stm32f7xx_hal_uart.h"

#pragma once

#include "constants.h"
#include "directionLabel.h"
#include "packet.h"
extern UART_HandleTypeDef huart5;

char* cmd = "AT\r\n";
char* cmd_mac_addr = "AT+UUID?\r\n";
char* iron_man = "I am iron man, I am iron man, I am iron man I am iron…..\r\n";
uint8_t buffer[16];  // Smaller buffer for simple responses

/** @brief Main Bluetooth Test. */
void main_bluetooth_test() {
  setupPeripherals();

  VisualizationPacket vizPacket{};
  vizPacket.classification = ClassificationLabel::Clapping;
  vizPacket.direction = DirectionLabel::North;
  vizPacket.priority = 3U;

  float angle_rad = 0.0;

  while (1) {
    uint8_t connected = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
    if (connected == 0) {
      // Send exactly the length of the string (4 bytes: A, T, \r, \n)
      HAL_UART_Transmit(&huart5, (uint8_t*)cmd, strlen(cmd), 100);

      // Clear buffer
      memset(buffer, 0, sizeof(buffer));

      // 4 bytes for "OK\r\n")
      HAL_UART_Receive(&huart5, buffer, 4, 1000);

      if (!(buffer[0] == 'O' && buffer[1] == 'K' && buffer[2] == '\r' &&
            buffer[3] == '\n')) {
        printf("fail");  // something wrong happened.
      }

      HAL_Delay(100);
    } else if (connected == 1) {
      angle_rad += PI_32 / 16.0;
      vizPacket.direction = angleToDirection(angle_rad);

      std::array<uint8_t, PACKET_BYTE_SIZE> packet = createPacket(vizPacket);
      HAL_UART_Transmit(&huart5, packet.data(), packet.size(), 100);
      HAL_Delay(100);
    }
  }
}
