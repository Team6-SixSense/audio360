/**
 ******************************************************************************
 * @file    runtime_android_comm.hpp
 * @brief   Runtime code for running android communication.
 ******************************************************************************
 */

#pragma once

#include <cstdint>

#include "constants.h"
#include "packet.h"
#include "peripheral.h"
#include "usb_host.h"
#include "usbh_aoa.h"

void mainAndroidComm() {
  VisualizationPacket vizPacket{};
  vizPacket.classification = ClassificationLabel::CarHorn;
  vizPacket.direction = DirectionLabel::North;
  vizPacket.priority = 3U;

  // Send packet over USB CDC.
  float angle_rad = 0.0;
  while (1) {


    MX_USB_HOST_Process();

    std::array<uint8_t, PACKET_BYTE_SIZE> packet = createPacket(vizPacket);

    USBH_AOA_Transmit(packet.data(), packet.size());

    // Update direction.
    angle_rad += PI_32 / 16.0;
    vizPacket.direction = angleToDirection(angle_rad);

    HAL_Delay(100);
  }
}
