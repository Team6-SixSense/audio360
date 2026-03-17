/**
 ******************************************************************************
 * @file    doaSmoother.cpp
 * @brief   DOA angle smoothing filter implementation.
 ******************************************************************************
 */

#include "doaSmoother.h"

#include <algorithm>
#include <cmath>

#include "angles.hpp"
#include "constants.h"

DoASmoother::DoASmoother() : currentAvg_(0.0f) {}

float DoASmoother::shortestAngleDiff(float a, float b) const {
  float diff = a - b;
  while (diff > PI_32) {
    diff -= TWO_PI_32;
  }
  while (diff < -PI_32) {
    diff += TWO_PI_32;
  }
  return diff;
}

bool DoASmoother::isWithinThreshold(float a, float b) const {
  float diffRad = std::abs(shortestAngleDiff(a, b));
  float thresholdRad = degreeToRad(JUMP_THRESHOLD_DEG);
  return diffRad <= thresholdRad;
}

float DoASmoother::averageOf(const std::deque<float>& buf) const {
  if (buf.empty()) {
    return 0.0f;
  }
  float sum = 0.0f;
  for (float v : buf) {
    sum += v;
  }
  return sum / static_cast<float>(buf.size());
}

float DoASmoother::update(float angle_rad) {
  angle_rad = normalizeAngleRad(angle_rad);

  if (active_.empty()) {
    active_.push_back(angle_rad);
    currentAvg_ = angle_rad;
    return angle_rad;
  }

  if (isWithinThreshold(angle_rad, currentAvg_)) {
    candidate_.clear();
    active_.push_front(angle_rad);
    if (active_.size() > MAX_WINDOW) {
      active_.pop_back();
    }
    currentAvg_ = averageOf(active_);
    return currentAvg_;
  }

  candidate_.push_back(angle_rad);
  if (candidate_.size() >= SWITCH_THRESHOLD) {
    active_.clear();
    for (float v : candidate_) {
      active_.push_back(v);
    }
    candidate_.clear();
    currentAvg_ = averageOf(active_);
    return currentAvg_;
  }

  return currentAvg_;
}
