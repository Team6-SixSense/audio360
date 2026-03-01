/**
 ******************************************************************************
 * @file    system_fault_state_machine_test.cpp
 * @brief   Unit tests for verifying system fault manager's state machine.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <vector>

#include "system_fault_manager.h"
#include "system_fault_states.h"

struct ErrorStatesParamType {
  bool hardwareError;  // true for hardware error.

  bool doaError;  // true for DOA error.

  bool classificationError;  // true for audio classification error.

  SystemFaultState expectedState;  // Expected system fault state for
                                   // combination of errors above.
};

/** @brief System Fault Manager module for testing. */
class SystemFaultStateMachineTest
    : public SystemFaultManager,
      public ::testing::TestWithParam<ErrorStatesParamType> {};

/** @brief Verify that the system fault manager's state machine provide the
 * correct system state based on the the error flags. */
TEST_P(SystemFaultStateMachineTest, CheckStateMachineStates) {
  ErrorStatesParamType params = GetParam();

  // Set errors.
  if (params.hardwareError) {
    this->reportHardwareError();
  } else {
    this->clearHardwareError();
  }

  if (params.doaError) {
    this->reportDoaError();
  } else {
    this->clearDoaError();
  }

  if (params.classificationError) {
    this->reportClassificationError();
  } else {
    this->clearClassficationError();
  }

  // Run the state machine and assert the correct state.
  this->runStateMachine();
  ASSERT_EQ(this->getSystemFaultState(), params.expectedState);
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(
    SystemErrors, SystemFaultStateMachineTest,
    ::testing::Values(
        ErrorStatesParamType{false, false, false, NO_FAULT},
        ErrorStatesParamType{false, false, true, CLASSIFICATION_FAULT},
        ErrorStatesParamType{false, true, false, DIRECTIONAL_ANALYSIS_FAULT},
        ErrorStatesParamType{false, true, true, DIRECTIONAL_ANALYSIS_FAULT},
        ErrorStatesParamType{true, false, false, HARDWARE_FAULT},
        ErrorStatesParamType{true, false, true, HARDWARE_FAULT},
        ErrorStatesParamType{true, true, false, HARDWARE_FAULT},
        ErrorStatesParamType{true, true, true, HARDWARE_FAULT}));
