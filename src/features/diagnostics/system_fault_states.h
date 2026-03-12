/**
 ******************************************************************************
 * @file    system_fault_states.h
 * @brief   System Fault States header.
 ******************************************************************************
 */

#pragma once

/** @brief System fault state. */
enum SystemFaultState {
  NO_FAULT = 0,
  HARDWARE_FAULT = 1,
  CLASSIFICATION_FAULT = 2,
  DIRECTIONAL_ANALYSIS_FAULT = 3
};
