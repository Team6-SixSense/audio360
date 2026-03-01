/**
 ******************************************************************************
 * @file    system_fault_manager.cpp
 * @brief   System Fault Manager source code.
 ******************************************************************************
 */

#include "system_fault_manager.h"

#include "logging.hpp"
#include "packet.h"

#ifdef STM_BUILD
#include "stm32f7xx_hal_cortex.h"
#endif

#ifdef BUILD_GLASSES_HOST
#include "usb_host.h"
#include "usbh_aoa.h"
#endif

SystemFaultManager::SystemFaultManager() {}

void SystemFaultManager::handlePeripheralSetupFaults(
    std::set<PeripheralError>* errors) {
  if (errors->empty()) {
    this->clearHardwareError();
    return;
  }

  this->reportHardwareError();

  for (PeripheralError error : *errors) {
    std::string errorString = peripheralErrorStrings[static_cast<int>(error)];
    switch (error) {
      case HAL_RCC_OSCILLATOR_INIT_FAIL:
      case HAL_PWR_ENABLE_OVERDRIVE_FAIL:
      case HAL_RCC_CLOCK_CONFIG_FAIL:
      case HAL_RCC_PERI_CLOCK_CONFIG_FAIL:
#ifdef STM_BUILD
        // Reboot MCU.
        HAL_NVIC_SystemReset();
#endif
      case HAL_SPI_INIT_FAIL:
      case HAL_SAI_A1_INIT_FAIL:
      case HAL_SAI_B1_INIT_FAIL:
      case HAL_SAI_A2_INIT_FAIL:
      case HAL_SAI_B2_INIT_FAIL:
      case HAL_UART_INIT_FAIL:
      case HAL_PCD_INIT:
      case SD_MOUNT_FAIL:
      case USB_DEVICE_INIT_FAIL:
      case USB_DEVICE_START_FAIL:
      case USB_HOST_INIT_FAIL:
      case INVALID_USB_SPEED:
      case DMA_INIT:

        // Enter unrecoverable state.
        enterUnrecoverableState(errorString);
      default:
        continue;
    }
  }
}

void SystemFaultManager::runStateMachine() {
  // System Fault priority:
  // 1. Hardware fault since software depends on hardware.
  // 2. DOA fault since safety critical feature if hardware is working
  // 3. Classification

  if (this->hardwareError) {
    this->state = HARDWARE_FAULT;
  } else if (this->doaError) {
    this->state = DIRECTIONAL_ANALYSIS_FAULT;
  } else if (this->classificationError) {
    this->state = CLASSIFICATION_FAULT;
  } else {
    this->state = NO_FAULT;
  }
}

void SystemFaultManager::reportHardwareError() { this->hardwareError = true; }

void SystemFaultManager::clearHardwareError() { this->hardwareError = false; }

void SystemFaultManager::reportClassificationError() {
  this->classificationError = true;
}

void SystemFaultManager::clearClassficationError() {
  this->classificationError = false;
}

void SystemFaultManager::reportDoaError() { this->doaError = true; }

void SystemFaultManager::clearDoaError() { this->doaError = false; }

SystemFaultState SystemFaultManager::getSystemFaultState() {
  return this->state;
}

void SystemFaultManager::updateFaultState(SystemFaultState faultState) {
  this->state = faultState;
}

void SystemFaultManager::enterUnrecoverableState(std::string error) {
  ERROR("Entering unrecoverable state due to %s", error);

#ifdef BUILD_GLASSES_HOST
  // Transmit system fault error to visualization before entering unrecoverable
  // state.
  VisualizationPacket vizPacket{};
  vizPacket.systemFaultState = this->state;
  std::array<uint8_t, PACKET_BYTE_SIZE> packet = createPacket(vizPacket);
  USBH_AOA_Transmit(packet.data(), packet.size());
#endif

  // Enter infinite loop to prevent any further execution.
  while (true) {
  }
}
