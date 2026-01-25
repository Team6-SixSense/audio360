/**
 ******************************************************************************
 * @file    system_fault_manager_test.cpp
 * @brief   Unit tests for verifying system fault manager module.
 ******************************************************************************
 */

#include "system_fault_manager.h"

#include <gtest/gtest.h>

struct PeripheralErrorParamType {
  PeripheralError error;  // Peripheral error.
};

/** @brief Spy class of System Fault Manager module. */
class SystemFaultManagerTest
    : public SystemFaultManager,
      public ::testing::TestWithParam<PeripheralErrorParamType> {
 public:
  void enterUnrecoverableState(std::string error) override {
    unrecoverableCalled = true;
    lastError = error;
  }

  /** @brief True if unrecoverable error has been reached. */
  bool unrecoverableCalled = false;

  /** @brief Recent error string. */
  std::string lastError;
};

/** @brief Verify that nothing happens when no pheripheral errors occur. */
TEST_F(SystemFaultManagerTest, NoPheripheralErrors) {
  std::set<PeripheralError> pheripheralErrors{};
  this->handlePeripheralSetupFaults(&pheripheralErrors);
  EXPECT_FALSE(unrecoverableCalled);
}

/** @brief Verify that unrecoverable state is entered when specific pheripheral
 * errors occur. */
TEST_P(SystemFaultManagerTest, PheripheralErrorsTriggerUnrecoverableState) {
  PeripheralErrorParamType params = GetParam();
  std::set<PeripheralError> pheripheralErrors = {params.error};

  this->handlePeripheralSetupFaults(&pheripheralErrors);

  EXPECT_TRUE(unrecoverableCalled);
  EXPECT_EQ(lastError, peripheralErrorStrings[static_cast<int>(params.error)]);
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(
    PheripheralErrors, SystemFaultManagerTest,
    ::testing::Values(PeripheralErrorParamType{HAL_RCC_OSCILLATOR_INIT_FAIL},
                      PeripheralErrorParamType{HAL_PWR_ENABLE_OVERDRIVE_FAIL},
                      PeripheralErrorParamType{HAL_RCC_CLOCK_CONFIG_FAIL},
                      PeripheralErrorParamType{HAL_RCC_PERI_CLOCK_CONFIG_FAIL},
                      PeripheralErrorParamType{HAL_SPI_INIT_FAIL},
                      PeripheralErrorParamType{HAL_SAI_A1_INIT_FAIL},
                      PeripheralErrorParamType{HAL_SAI_B1_INIT_FAIL},
                      PeripheralErrorParamType{HAL_SAI_A2_INIT_FAIL},
                      PeripheralErrorParamType{HAL_SAI_B2_INIT_FAIL},
                      PeripheralErrorParamType{HAL_UART_INIT_FAIL},
                      PeripheralErrorParamType{HAL_PCD_INIT},
                      PeripheralErrorParamType{SD_MOUNT_FAIL},
                      PeripheralErrorParamType{USB_DEVICE_INIT_FAIL},
                      PeripheralErrorParamType{USB_DEVICE_START_FAIL},
                      PeripheralErrorParamType{USB_HOST_INIT_FAIL},
                      PeripheralErrorParamType{INVALID_USB_SPEED},
                      PeripheralErrorParamType{DMA_INIT}));
