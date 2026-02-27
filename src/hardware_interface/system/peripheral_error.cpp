/**
 ******************************************************************************
 * @file    peripheral_error.cpp
 * @brief   This file contains the peripheral errors source code.
 ******************************************************************************
 */

#include "peripheral_error.h"

#include "peripheral_error.hpp"

static std::set<PeripheralError> errors;

void Report_Error(PeripheralError error) {
  // Store error to set of errors that has occurred.
  errors.insert(error);
}

std::set<PeripheralError>* getPeripheralErrors() { return &errors; }
