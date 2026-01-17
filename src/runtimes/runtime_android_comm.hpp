/**
 ******************************************************************************
 * @file    runtime_android_comm.hpp
 * @brief   Runtime code for running android communication.
 ******************************************************************************
 */

#pragma once

#include <cstdint>

#include "constants.h"
#include "logging.hpp"
#include "packet.h"
#include "peripheral.h"
#include "usbd_cdc_if.h"

void mainAndroidComm() {
  VisualizationPacket vizPacket{};
  vizPacket.classification = ClassificationLabel::CarHorn;
  vizPacket.direction = DirectionLabel::North;
  vizPacket.priority = 3U;

  // Send packet over USB CDC.
  float angle_rad = 0.0;
  while (1) {
    std::array<uint8_t, PACKET_BYTE_SIZE> packet = createPacket(vizPacket);
    CDC_Transmit_FS(packet.data(), PACKET_BYTE_SIZE);

    // Update direction.
    angle_rad += PI_32 / 16.0;
    vizPacket.direction = angleToDirection(angle_rad);

    HAL_Delay(100);
  }
}
