/**
 ******************************************************************************
 * @file    peripheral.h
 * @brief   This file contains the headers of the peripherals.
 ******************************************************************************
 */

#pragma once

#ifdef STM_BUILD

#include "stm32f7xx_hal.h"
#include "stm32f7xx_nucleo_144.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sets up required peripherals on the hardware with proper clock
 * configurations. This function must be called before using any hardware
 * related code.
 */
void setupPeripherals();

/**
 * @brief Output for when errors occurs. Will output LED3. @ref setupPeripherals
 * must be called before calling this function
 */
void Error_Handler();

void SystemClock_Config(void);


#ifdef __cplusplus
}
#endif

#endif
