/**
 ******************************************************************************
 * @file    doaSmoother.h
 * @brief   DOA angle smoothing filter to reduce jitter.
 ******************************************************************************
 */

#pragma once

#include <cstddef>
#include <deque>

/**
 * @brief Smooths DOA angle outputs using a sliding average.
 * Ignores 1-2 frame jumps; tracks new locations after 3+ consecutive frames.
 */
class DoASmoother {
 public:
  DoASmoother();

  /**
   * @brief Update with new angle and return smoothed value.
   * @param angle_rad Angle in radians [0, 2*pi).
   * @return Smoothed angle in radians.
   */
  float update(float angle_rad);

 private:
  static constexpr size_t MAX_WINDOW = 5;
  static constexpr size_t SWITCH_THRESHOLD = 3;
  static constexpr float JUMP_THRESHOLD_DEG = 15.0f;

  std::deque<float> active_;
  std::deque<float> candidate_;
  float currentAvg_;

  bool isWithinThreshold(float a, float b) const;
  float averageOf(const std::deque<float>& buf) const;
  float shortestAngleDiff(float a, float b) const;
};
