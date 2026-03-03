/**
 ******************************************************************************
 * @file    sign_extend_test.cpp
 * @brief   Unit tests for sign extension.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "bit_operations.hpp"

/** @brief Struct for parameterized testing. */
struct SignExtendParamType {
  uint32_t input;  // Input number to sign extend.

  int32_t output;  // Expected return for sign extended.
};

/** @brief Parameterized test class for GCC PhaT angle accuracy. */
class SignExtendTest : public ::testing::TestWithParam<SignExtendParamType> {};

/** @brief Verify that sign extend correctly. */
TEST_P(SignExtendTest, SignExtendTest) {
  SignExtendParamType param = GetParam();

  EXPECT_EQ(signExtend24To32(param.input), param.output);
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(
    SignExtend, SignExtendTest,
    ::testing::Values(
        SignExtendParamType{0x00000001, static_cast<int32_t>(0x00000001)},
        SignExtendParamType{0x00040000, static_cast<int32_t>(0x00040000)},
        SignExtendParamType{0x00800000, static_cast<int32_t>(0xFF800000)}));
