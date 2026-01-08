/**
 ******************************************************************************
 * @file    peripheral_error.hpp
 * @brief   This file contains the peripheral errors C++ header.
 ******************************************************************************
 */

#pragma once

#include <set>

#include "peripheral_error.h"

/** @brief Get the set of peripheral errors that has occured. */
std::set<PeripheralError>* getPeripheralErrors();
