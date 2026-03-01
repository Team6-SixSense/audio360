/**
 ******************************************************************************
 * @file    peripheral_error_test.cpp
 * @brief   Unit tests pheripheral errors.
 ******************************************************************************
 */

#include "peripheral_error.h"

#include <gtest/gtest.h>

#include <vector>

#include "peripheral_error.hpp"

/** @brief Verify that reporting peripheral error is saved to a set for data
 * integrity. */
TEST(PeripheralErrorTest, ReportPeripheralError) {
  std::set<PeripheralError>* errors = getPeripheralErrors();
  ASSERT_TRUE(errors->empty());

  // Report peripheral errors.
  std::vector<PeripheralError> occurredErrors{HAL_PWR_ENABLE_OVERDRIVE_FAIL,
                                              HAL_SAI_A1_INIT_FAIL,
                                              HAL_PCD_INIT, INVALID_USB_SPEED};
  // Assert the reported peripheral errors are stored.
  for (PeripheralError error : occurredErrors) {
    Report_Error(error);
    ASSERT_EQ(errors->count(error), 1);
  }
}
