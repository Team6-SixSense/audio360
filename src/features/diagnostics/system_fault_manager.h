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

  /** @brief Runs System Fault Manager internal state machine to determine the
   * current system fault state. */
  void runStateMachine();

  /** @brief Reports hardware error. */
  void reportHardwareError();

  /** @brief Clears hardware error. */
  void clearHardwareError();

  /** @brief Reports audio classification error. */
  void reportClassificationError();

  /** @brief Clears audio classification error. */
  void clearClassficationError();

  /** @brief Reports DOA error. */
  void reportDoaError();

  /** @brief Clears DOA error. */
  void clearDoaError();

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

  /** @brief True if there is a hardware error. False is there are no errors
   * associated to hardware. */
  bool hardwareError{false};

  /** @brief True if there is an audio classification error. False is there are
   * no errors associated to audio classification. */
  bool classificationError{false};

  /** @brief True if there is a DOA error. False is there are no errors
   * associated to DOA. */
  bool doaError{false};
};
