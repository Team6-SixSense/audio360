/**
 ******************************************************************************
 * @file    packet.h
 * @brief   Packet for visualization header code
 ******************************************************************************
 */

#pragma once

#include <array>
#include <cstdint>

#include "classificationLabel.h"
#include "directionLabel.h"
#include "system_fault_states.h"

const size_t PACKET_BYTE_SIZE = 5;

/** @brief Struct for representing the packet being sent to the visualization
 * module. */
struct VisualizationPacket {
  /** @brief Starting byte of packet. */
  const uint8_t startByte = 0xAA;

  /** @brief Classification of audio source. */
  ClassificationLabel classification{ClassificationLabel::Unknown};

  /** @brief Direction of the audio source. */
  DirectionLabel direction{DirectionLabel::None};

  /** @brief Current system fault state. */
  SystemFaultState systemFaultState{SystemFaultState::NO_FAULT};

  /** @brief Priority of packet. */
  uint8_t priority{0U};
};

/**
 * @brief Create a packet buffer that can be sent to visualization module.
 *
 * @param vizPacket Visualization packet struct containing all the information
 * to
 * @return std::array<uint8_t, PACKET_BYTE_SIZE>
 */
inline std::array<uint8_t, PACKET_BYTE_SIZE> createPacket(
    VisualizationPacket vizPacket) {
  std::array<uint8_t, PACKET_BYTE_SIZE> packet = {
      vizPacket.startByte, static_cast<uint8_t>(vizPacket.classification),
      static_cast<uint8_t>(vizPacket.direction),
      static_cast<uint8_t>(vizPacket.systemFaultState), vizPacket.priority};
  return packet;
}
