/**
 ******************************************************************************
 * @file    peripheral_error.h
 * @brief   This file contains the peripheral errors C header.
 ******************************************************************************
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** @brief All the errors associated to peripherals. */
typedef enum PeripheralError {
  HAL_RCC_OSCILLATOR_INIT_FAIL,    // Failed to initialize RCC oscillators.
  HAL_PWR_ENABLE_OVERDRIVE_FAIL,   // Failed to enable over-drive mode.
  HAL_RCC_CLOCK_CONFIG_FAIL,       // Failed to configure RCC clock.
  HAL_RCC_PERI_CLOCK_CONFIG_FAIL,  // Failed to configure peripherals' clock.
  HAL_SPI_INIT_FAIL,               // Failed to initialize SPI.
  HAL_SAI_A1_INIT_FAIL,            // Failed to initialize SAI block A1.
  HAL_SAI_B1_INIT_FAIL,            // Failed to initialize SAI block B1.
  HAL_SAI_A2_INIT_FAIL,            // Failed to initialize SAI block A2.
  HAL_SAI_B2_INIT_FAIL,            // Failed to initialize SAI block B2.
  HAL_UART_INIT_FAIL,              // Failed to initialize UART.
  HAL_PCD_INIT,                    // Failed to initialize PCD.
  SD_MOUNT_FAIL,                   // Failed to mount SD card.
  USB_DEVICE_INIT_FAIL,            // Failed to initialized USB device.
  USB_DEVICE_START_FAIL,           // Failed to start USB device.
  USB_HOST_INIT_FAIL,              // Failed to start USB host.
  INVALID_USB_SPEED,               // Invalid USB speed.
  DMA_INIT,                        // Failed to initialize DMA.
} PeripheralError;

static const char* peripheralErrorStrings[] = {"HAL_RCC_OSCILLATOR_INIT_FAIL",
                                               "HAL_PWR_ENABLE_OVERDRIVE_FAIL",
                                               "HAL_RCC_CLOCK_CONFIG_FAIL",
                                               "HAL_RCC_PERI_CLOCK_CONFIG_FAIL",
                                               "HAL_SPI_INIT_FAIL",
                                               "HAL_SAI_A1_INIT_FAIL",
                                               "HAL_SAI_B1_INIT_FAIL",
                                               "HAL_SAI_A2_INIT_FAIL",
                                               "HAL_SAI_B2_INIT_FAIL",
                                               "HAL_UART_INIT_FAIL",
                                               "HAL_PCD_INIT",
                                               "SD_MOUNT_FAIL",
                                               "USB_DEVICE_INIT_FAIL",
                                               "USB_DEVICE_START_FAIL",
                                               "USB_HOST_INIT_FAIL",
                                               "INVALID_USB_SPEED",
                                               "DMA_INIT"};

/**
 * @brief Report when an error related to pheripheral has occured.
 *
 * @param error Pheripheral error that has occurred.
 */
void Report_Error(PeripheralError error);

#ifdef __cplusplus
}
#endif
