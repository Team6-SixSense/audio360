/**
 ******************************************************************************
 * @file    angles.hpp
 * @brief   Angle operations header and source.
 ******************************************************************************
 */

#pragma once

#include <cmath>

#include "constants.h"

/**
 * @brief Converts angles from degress to radians.
 *
 * @param angle Angle in degrees
 * @return float Radian equivalent.
 */
inline float degreeToRad(float angle) { return angle * PI_32 / 180.0f; }

/**
 * @brief Represent angle in range of [0, 2*PI).
 *
 * @param angle angle to normalize.
 * @return float angle representation in range of [0, 2*PI).
 */
inline float normalizeAngleRad(float angle) {
  if (angle < 0 || angle >= TWO_PI_32) {
    angle -= TWO_PI_32 * std::floor(angle / TWO_PI_32);
  }

  return angle;
}
