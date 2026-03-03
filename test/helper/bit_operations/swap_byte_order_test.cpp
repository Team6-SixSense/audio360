/**
 ******************************************************************************
 * @file    swap_byte_order_test.cpp
 * @brief   Unit tests for byte swapping.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "bit_operations.hpp"

/** @brief Struct for parameterized testing. */
struct SwapByteParamType {
  uint32_t input;  // Input number to byte swap.

  uint32_t output;  // Expected return for byte swap.
};

/** @brief Parameterized test class for GCC PhaT angle accuracy. */
class SwapByteTest : public ::testing::TestWithParam<SwapByteParamType> {};

/** @brief Verify that sign extend correctly. */
TEST_P(SwapByteTest, SwapByteTest) {
  SwapByteParamType param = GetParam();

  EXPECT_EQ(swapByteOrder32(param.input), param.output);
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(
    SwapByte, SwapByteTest,
    ::testing::Values(SwapByteParamType{0x00000001, 0x01000000},
                      SwapByteParamType{0x12345678, 0x78563412},
                      SwapByteParamType{0xFEDCBA10, 0x10BADCFE}));
