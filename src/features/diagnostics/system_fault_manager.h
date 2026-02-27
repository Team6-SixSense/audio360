/**
 ******************************************************************************
 * @file    system_fault_manager.h
 * @brief   System Fault Manager header.
 ******************************************************************************
 */

#pragma once

#include <queue>
#include <set>
#include <string>

#include "peripheral_error.h"
#include "system_fault_states.h"

/** @brief Module to monitors system health and manage critical system faults.
 */
class SystemFaultManager {
 public:
  /** @brief Construct a new System Fault Manager object. */
  SystemFaultManager();

  /**
   * @brief Handles faults that occurred during peripheral set-up.
   *
   * @param errors Set of peripheral set-up errors.
   */
  void handlePeripheralSetupFaults(std::set<PeripheralError>* errors);

  /**
   * @brief Return the current system fault state.
   *
   * @return SystemFaultState The current system fault state.
   */
  SystemFaultState getSystemFaultState();

 protected:
  /**
   * @brief Setter for system fault state for testing purposes.
   *
   * @param faultState The fault state to set to.
   */
  void updateFaultState(SystemFaultState faultState);

 private:
  /**
   * @brief Enter unrecoverable state. Only call this when the system has
   * no way of fixing itself. This is meant to control the system behaviour when
   * critical faults occur that prevent from performing core functionalities.
   *
   * @param error Error that has caused to enter the unrecoverable state.
   */
  virtual void enterUnrecoverableState(std::string error);

  /** @brief Current system fault state. */
  SystemFaultState state{NO_FAULT};
};
