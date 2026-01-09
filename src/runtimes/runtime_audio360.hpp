/**
 ******************************************************************************
 * @file    runtime_audio360.hpp
 * @brief   FFT runtime code.
 ******************************************************************************
 */

#pragma once

#include "logging.hpp"
#include "peripheral.h"
#include "peripheral_error.hpp"
#include "system_fault_manager.h"

void mainAudio360() {
  INFO("Running Audio360.");

  // Set-up peripherals. Must call before any hardware function calls.
  setupPeripherals();

  SystemFaultManager systemFaultManager;
  systemFaultManager.handlePeripheralSetupFaults(getPeripheralErrors());

  while (true) {
    // TODO: While loop may not adhere to timing constraints. Need a better way
    // to control execution.

    // Insert Audio360 software layer code here.
  }
}
