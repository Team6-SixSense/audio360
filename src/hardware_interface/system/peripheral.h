/**
 ******************************************************************************
 * @file    peripheral.h
 * @brief   This file contains the headers of the peripherals.
 ******************************************************************************
 */

#pragma once

#ifdef STM_BUILD

#include <stdio.h>

#include "peripheral_error.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_nucleo_144.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sets up required peripherals on the hardware with proper clock
 * configurations. This function must be called before using any hardware
 * related code.
 */
void setupPeripherals();

void SystemClock_Config(void);

/** @brief Get handle to SAI 1 module block A. */
SAI_HandleTypeDef* getSAI1A_Handle();

/** @brief Get handle to SAI 1 module block B. */
SAI_HandleTypeDef* getSAI1B_Handle();

/** @brief Get handle to SAI 2 module block A. */
SAI_HandleTypeDef* getSAI2A_Handle();

/** @brief Get handle to SAI 2 module block B. */
SAI_HandleTypeDef* getSAI2B_Handle();

#ifdef __cplusplus
}
#endif

#endif
