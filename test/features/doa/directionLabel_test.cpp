/**
 ******************************************************************************
 * @file    directionLabel.cpp
 * @brief   Unit tests for verifying angle to direction label is correct.
 ******************************************************************************
 */

#include "directionLabel.h"

#include <gtest/gtest.h>

#include "constants.h"

/** @brief Angle between [-pi/8, pi/8] should be North direction. */
TEST(DirectionLabelTest, NorthLabel) {
  ASSERT_EQ(angleToDirection(0.0), DirectionLabel::North);
  ASSERT_EQ(angleToDirection(15.5 * PI_32 / 8.0), DirectionLabel::North);
}

/** @brief Angle between [pi/8, 3*pi/8] should be North West direction. */
TEST(DirectionLabelTest, NorthWestLabel) {
  ASSERT_EQ(angleToDirection(PI_32 / 4.0), DirectionLabel::NorthWest);
}

/** @brief Angle between [3*pi/8, 5*pi/8] should be West direction. */
TEST(DirectionLabelTest, WestLabel) {
  ASSERT_EQ(angleToDirection(PI_32 / 2.0), DirectionLabel::West);
}

/** @brief Angle between [5*pi/8, 7*pi/8] should be South West direction. */
TEST(DirectionLabelTest, SouthWestLabel) {
  ASSERT_EQ(angleToDirection(3.0 * PI_32 / 4.0), DirectionLabel::SouthWest);
}

/** @brief Angle between [7*pi/8, 9*pi/8] should be South direction. */
TEST(DirectionLabelTest, SouthLabel) {
  ASSERT_EQ(angleToDirection(PI_32), DirectionLabel::South);
}

/** @brief Angle between [9*pi/8, 11*pi/8] should be South East direction. */
TEST(DirectionLabelTest, SouthEastLabel) {
  ASSERT_EQ(angleToDirection(5.0 * PI_32 / 4.0), DirectionLabel::SouthEast);
}

/** @brief Angle between [11*pi/8, 13*pi/8] should be East direction. */
TEST(DirectionLabelTest, EastLabel) {
  ASSERT_EQ(angleToDirection(3.0 * PI_32 / 2.0), DirectionLabel::East);
}

/** @brief Angle between [13*pi/8, 15*pi/8] should be North East direction. */
TEST(DirectionLabelTest, NorhtEastLabel) {
  ASSERT_EQ(angleToDirection(7.0 * PI_32 / 4.0), DirectionLabel::NorthEast);
}
