/**
 ******************************************************************************
 * @file    main.cpp
 * @brief   Main entry code for Audio360.
 ******************************************************************************
 */

#ifdef STM_BUILD
#include "hardware_interface/system/peripheral.h"
#else
#include "features/signal_processing/fft.h"
#endif

#define DEBUG_I2S_MIC

#include "helper/logging/logging.hpp"
#include <stdio.h>

int main() {

#ifdef STM_BUILD
  // Set-up pheripherals. Must call before any hardware function calls.
  setupPeripherals();
#else
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()));
#endif

  while (1) {
    INFO("Hello SixSense!");

#ifdef DEBUG_I2S_MIC
    uint32_t recvd_data[2];
    HAL_SAI_StateTypeDef state = HAL_SAI_GetState(getSAI_Handle());

    SAI_HandleTypeDef * handle =  getSAI_Handle();
    HAL_StatusTypeDef status = HAL_SAI_Receive(handle, (uint8_t*)recvd_data, 2, 100);
#endif

    bool flag_cnrdy = __HAL_SAI_GET_FLAG(getSAI_Handle(), SAI_FLAG_CNRDY);
    bool flag_overrun = __HAL_SAI_GET_FLAG(getSAI_Handle(), SAI_FLAG_OVRUDR);
    bool flag_wckcfg = __HAL_SAI_GET_FLAG(getSAI_Handle(), SAI_FLAG_WCKCFG);
    bool flag_freq = __HAL_SAI_GET_FLAG(getSAI_Handle(), SAI_FLAG_FREQ);
    bool flag_afsdet = __HAL_SAI_GET_FLAG(getSAI_Handle(), SAI_FLAG_AFSDET);
    bool flag_lfsdet = __HAL_SAI_GET_FLAG(getSAI_Handle(), SAI_FLAG_LFSDET);
    bool flag_mutedet = __HAL_SAI_GET_FLAG(getSAI_Handle(), SAI_FLAG_MUTEDET);



    DEBUG("Audio360 is running.");
  }

  return 0;
}
