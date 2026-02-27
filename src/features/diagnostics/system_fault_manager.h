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

/** @brief System fault state. */
enum SystemFaultState {
  NO_FAULT,
  HARDWARE_FAULT,
  CLASSIFICATION_FAULT,
  DIRECTIONAL_ANALYSIS_FAULT
};

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
