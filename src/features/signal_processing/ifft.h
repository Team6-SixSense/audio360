/**
 ******************************************************************************
 * @file    ifft.h
 * @brief   Inverse Fast Fourier Transform (IFFT) header
 ******************************************************************************
 */

#pragma once

#include "arm_math.h"

#ifdef STM_BUILD
// stm32f767xx include must be first include to use CMSIS library.
#include "stm32f767xx.h"

#endif
#include <cmath>
#include <vector>

#include "frequencyDomain.h"
#include "logging.hpp"

/** @brief Inverse Fast Fourier Transform (IFFT) class. */
class IFFT {
 public:
  /**
   * @brief Construct a new IFFT object.
   *
   * @param numSamples The number of time samples after IFFT. In otherwords
   * (N - 1) * 2 where N is the number of elements in frequency domain.
   */
  IFFT(uint16_t numSamples);

  /** @brief Copy constructor. */
  IFFT(const IFFT& other);

  /** @brief Copy assignment operator. */
  IFFT& operator=(const IFFT& other);

  /** @brief Destroy the IFFT object. */
  ~IFFT();

  /**
   * @brief Converts input frequency to the time domain.
   *
   * @param frequency Frequency.

   * @return  The signal represented in the frequency domain.
   */
  std::vector<float> frequencyToTime(const FrequencyDomain& frequencyDomain);

 private:
  /** @brief Initializes FFT instance from CMSIS-DSP lib. */
  inline void initializeFFTInstance() {
    arm_status status =
        arm_rfft_fast_init_f32(&rfft_instance, this->numSamples);

    if (status != arm_status::ARM_MATH_SUCCESS) {
      ERROR("Error in initializing CMSIS DSP FFT. Error status code %d",
            status);
    }
  }

  /**
   * @brief Inserts the frequency to internal memory of this class.
   *
   * @param frequency Frequency.
   */
  void insertSignal(const FrequencyDomain& frequencyDomain);

  /** Scale the output time domain signal since CMSIS does not do it. */
  void scaleOutput();

  /** @brief The number of time samples after IFFT.*/
  uint16_t numSamples{0U};

  /** @brief input frequency. This memory is shared with fttw_plan plan. */
  float32_t* in;

  /** @brief output signal. This memory is shared with fttw_plan plan. */
  float32_t* out;

  /** @brief Real FFT instance for using CMSIS DSP library */
  arm_rfft_fast_instance_f32 rfft_instance;
};
