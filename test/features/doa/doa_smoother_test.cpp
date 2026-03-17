/**
 ******************************************************************************
 * @file    doa_smoother_test.cpp
 * @brief   Unit tests for DoASmoother.
 ******************************************************************************
 */

#include "doaSmoother.h"

#include <gtest/gtest.h>
#include <cmath>

#include "angles.hpp"

static float angularErrorRad(float a, float b) {
  float diff = std::abs(a - b);
  if (diff > 3.14159265f) {
    diff = 2.0f * 3.14159265f - diff;
  }
  return diff;
}

static bool withinDeg(float actual_rad, float expected_rad, float tol_deg) {
  float err = angularErrorRad(actual_rad, expected_rad);
  return radToDegree(err) <= tol_deg;
}

TEST(DoASmootherTest, FirstInput_ReturnsInput) {
  DoASmoother smoother;
  float out = smoother.update(0.0f);
  EXPECT_TRUE(withinDeg(out, 0.0f, 1.0f));
}

TEST(DoASmootherTest, StableInput_ReturnsAverage) {
  DoASmoother smoother;
  for (int i = 0; i < 5; i++) {
    float out = smoother.update(0.0f);
    EXPECT_TRUE(withinDeg(out, 0.0f, 1.0f));
  }
}

TEST(DoASmootherTest, SingleFrameJump_Ignored) {
  DoASmoother smoother;
  float deg0 = degreeToRad(0.0f);
  float deg30 = degreeToRad(30.0f);

  smoother.update(deg0);
  smoother.update(deg30);
  float out = smoother.update(deg0);
  EXPECT_TRUE(withinDeg(out, deg0, 5.0f)) << "Single jump should be ignored";
  out = smoother.update(deg0);
  EXPECT_TRUE(withinDeg(out, deg0, 5.0f));
}

TEST(DoASmootherTest, TwoFrameJump_Ignored) {
  DoASmoother smoother;
  float deg0 = degreeToRad(0.0f);
  float deg30 = degreeToRad(30.0f);

  smoother.update(deg0);
  smoother.update(deg30);
  smoother.update(deg30);
  float out = smoother.update(deg0);
  EXPECT_TRUE(withinDeg(out, deg0, 5.0f)) << "Two-frame jump should be ignored";
}

TEST(DoASmootherTest, ThreeFrameJump_StartsTracking) {
  DoASmoother smoother;
  float deg0 = degreeToRad(0.0f);
  float deg30 = degreeToRad(30.0f);

  smoother.update(deg0);
  smoother.update(deg0);
  smoother.update(deg30);
  smoother.update(deg30);
  float out = smoother.update(deg30);
  EXPECT_TRUE(withinDeg(out, deg30, 5.0f)) << "After 3 frames at 30°, should track";
}

TEST(DoASmootherTest, GradualChange_WithinThreshold) {
  DoASmoother smoother;
  float angles[] = {0.0f, 5.0f, 10.0f, 15.0f, 14.0f};
  for (float deg : angles) {
    float out = smoother.update(degreeToRad(deg));
    EXPECT_TRUE(withinDeg(out, degreeToRad(deg), 20.0f))
        << "Gradual change within ±15° should be tracked";
  }
}
