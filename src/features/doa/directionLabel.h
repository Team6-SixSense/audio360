/**
 ******************************************************************************
 * @file    directionLabel.h
 * @brief   Direction label header code
 ******************************************************************************
 */

#pragma once

#include "angles.hpp"
#include "constants.h"

/** @brief Enum holding all direction labels. */
enum class DirectionLabel {
  None = 0,
  North = 1,
  NorthWest = 2,
  West = 3,
  SouthWest = 4,
  South = 5,
  SouthEast = 6,
  East = 7,
  NorthEast = 8,
};

/**
 * @brief Returns the label corresponding to the angle.
 *
 * @param angle_rad Angle in radian.
 * @return DirectionLabel direction label corresponding to the angle.
 */
inline DirectionLabel angleToDirection(float angle_rad) {
  angle_rad = normalizeAngleRad(angle_rad);

  // Shift angle by pi/8 since North covers [-pi/8, pi/8].
  angle_rad += PI_32 / 8.0;

  int index = static_cast<int>(angle_rad / (PI_32 / 4.0)) % 8;
  index += 1;  // Add one since None is 0 in the enum.

  return static_cast<DirectionLabel>(index);
}
