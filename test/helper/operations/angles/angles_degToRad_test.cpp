/**
 ******************************************************************************
 * @file    angles_degToRad_test.cpp
 * @brief   Unit tests for angle conversion from degrees to radians.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "angles.hpp"
#include "constants.h"
#include "test_helper.h"

/** @brief Struct for parameterized testing. */
struct AngleParamType {
  int angle;  // Angle in degree.

  float expectedAngle_rad;  // Expected angle in radians.
};

/** @brief Parameterized test class for angle conversion from degrees to
 * radians. */
class DegToRadAngleTest : public ::testing::TestWithParam<AngleParamType> {};

/** @brief Given an angle in degrees, verify conversion to radians is correct.
 */
TEST_P(DegToRadAngleTest, DegToRadTest) {
  AngleParamType param = GetParam();

  EXPECT_NEAR(degreeToRad(param.angle), param.expectedAngle_rad,
              PRECISION_ERROR);
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(
    DegToRadAngleValues, DegToRadAngleTest,
    ::testing::Values(AngleParamType{0, 0.0}, AngleParamType{45, PI_32 / 4.0},
                      AngleParamType{90, PI_32 / 2.0},
                      AngleParamType{135, 3.0 * PI_32 / 4.0},
                      AngleParamType{180, PI_32},
                      AngleParamType{225, 5.0 * PI_32 / 4.0},
                      AngleParamType{270, 3.0 * PI_32 / 2.0},
                      AngleParamType{315, 7.0 * PI_32 / 4.0}));
