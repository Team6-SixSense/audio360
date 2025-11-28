/**
 ******************************************************************************
 * @file    stm32f7xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include "peripheral.h"
#ifdef STM_BUILD

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"

/******************************************************************************/
/*           Cortex-M7 Processor Interruption and Exception Handlers          */
/******************************************************************************/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) { HAL_IncTick(); }

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles USB On The Go FS global interrupt.
 */
void OTG_FS_IRQHandler(void) {
  /* USER CODE BEGIN OTG_FS_IRQn 0 */

  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */

  /* USER CODE END OTG_FS_IRQn 1 */
}

/**
 * @brief This function handles SAI1 global interrupt.
 */
void SAI1_IRQHandler(void) {
  /* USER CODE BEGIN SAI1_IRQn 0 */

  /* USER CODE END SAI1_IRQn 0 */
  HAL_SAI_IRQHandler(getSAI1A_Handle());
  /* USER CODE BEGIN SAI1_IRQn 1 */

  /* USER CODE END SAI1_IRQn 1 */
}

#endif
