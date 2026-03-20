/**
 ******************************************************************************
 * @file    packet_test.cpp
 * @brief   Unit tests for testing creation of visualization packets.
 ******************************************************************************
 */

#include "packet.h"

#include <gtest/gtest.h>

/** @brief Struct for parameterized testing. */
struct PacketParamType {
  ClassificationLabel classification;  // Classification of audio source

  DirectionLabel direction;  // Direction of the audio source

  SystemFaultState systemFaultState;  // Current system fault state

  std::array<uint8_t, PACKET_BYTE_SIZE> packetBytes;  // packet in bytes
};

/** @brief Parameterized test class for GCC PhaT angle accuracy. */
class PacketCreationTest : public ::testing::TestWithParam<PacketParamType> {};

/** @brief Verify packet creation function creates packet as per request. */
TEST_P(PacketCreationTest, CreatePackets) {
  PacketParamType param = GetParam();
  VisualizationPacket vizPacket{};
  vizPacket.classification = param.classification;
  vizPacket.direction = param.direction;
  vizPacket.systemFaultState = param.systemFaultState;

  // Create Packet.
  std::array<uint8_t, PACKET_BYTE_SIZE> packet = createPacket(vizPacket);

  // Assert packet creation is correct.
  EXPECT_EQ(packet, param.packetBytes);
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(
    PacketCreations, PacketCreationTest,
    ::testing::Values(PacketParamType{ClassificationLabel::Unknown,
                                      DirectionLabel::None,
                                      SystemFaultState::NO_FAULT,
                                      {0xAA, 0x00, 0x00, 0x00, 0x00}},
                      PacketParamType{ClassificationLabel::SomeoneTalking,
                                      DirectionLabel::North,
                                      SystemFaultState::HARDWARE_FAULT,
                                      {0xAA, 0x01, 0x01, 0x01, 0x0}},
                      PacketParamType{ClassificationLabel::SomeoneTalking,
                                      DirectionLabel::West,
                                      SystemFaultState::CLASSIFICATION_FAULT,
                                      {0xAA, 0x01, 0x03, 0x02, 0x00}},
                      PacketParamType{
                          ClassificationLabel::Siren,
                          DirectionLabel::North,
                          SystemFaultState::DIRECTIONAL_ANALYSIS_FAULT,
                          {0xAA, 0x02, 0x01, 0x03, 0x00}},
                      PacketParamType{ClassificationLabel::SmokeAlarm,
                                      DirectionLabel::West,
                                      SystemFaultState::CLASSIFICATION_FAULT,
                                      {0xAA, 0x03, 0x03, 0x02, 0x00}}));
