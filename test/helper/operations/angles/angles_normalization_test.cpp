/**
 ******************************************************************************
 * @file    angles_noralization_test.cpp
 * @brief   Unit tests for angles normalization.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "angles.hpp"
#include "constants.h"
#include "test_helper.h"

/** @brief Struct for parameterized testing. */
struct AngleParamType {
  float angle_rad;  // Angle in radian.

  float expectedAngle_rad;  // Expected angle in radians.
};

/** @brief Parameterized test class for angle normalization. */
class AngleNormalizationTest : public ::testing::TestWithParam<AngleParamType> {
};

/** @brief Given an angle in radians, verify that after normalization, the angle
 * is range of [0, 2*PI). */
TEST_P(AngleNormalizationTest, DegToRadTest) {
  AngleParamType param = GetParam();

  EXPECT_NEAR(normalizeAngleRad(param.angle_rad), param.expectedAngle_rad,
              PRECISION_ERROR);
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(
    DegAngleNormalizationValues, AngleNormalizationTest,
    ::testing::Values(AngleParamType{0.0, 0.0}, AngleParamType{TWO_PI_32, 0.0},
                      AngleParamType{PI_32 / 2.0, PI_32 / 2.0},
                      AngleParamType{PI_32, PI_32},
                      AngleParamType{3.0 * PI_32 / 2.0, 3.0 * PI_32 / 2.0},
                      AngleParamType{2.0 * TWO_PI_32, 0.0},
                      AngleParamType{5.0 * PI_32 / 2.0, PI_32 / 2.0},
                      AngleParamType{3.0 * PI_32, PI_32},
                      AngleParamType{7.0 * PI_32 / 2.0, 3.0 * PI_32 / 2.0},
                      AngleParamType{-3.0 * PI_32 / 2.0, PI_32 / 2.0},
                      AngleParamType{-1.0 * PI_32, PI_32},
                      AngleParamType{-1.0 * PI_32 / 2.0, 3.0 * PI_32 / 2.0}));
