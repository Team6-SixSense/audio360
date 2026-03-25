/**
 ******************************************************************************
 * @file    ifft.cpp
 * @brief   Inverse Fast Fourier Transform (IFFT) class
 ******************************************************************************
 */

#include "ifft.h"

#include <stdio.h>

#include <algorithm>
#include <cassert>
#include <cmath>

#include "constants.h"

/** @brief input frequency. This memory is statically allocated. */
float32_t IFFT::in[FFT_BUFFER_SIZE_IN] = {0.0f};

/** @brief output signal. This memory is statically allocated. */
float32_t IFFT::out[FFT_BUFFER_SIZE_OUT] = {0.0f};

IFFT::IFFT(uint16_t numSamples) : numSamples(numSamples) {
  this->initializeFFTInstance();
}

IFFT::IFFT(const IFFT& other) : numSamples(other.numSamples) {
  this->initializeFFTInstance();
}

IFFT::~IFFT() = default;

float* IFFT::frequencyToTime(const FrequencyDomain& frequencyDomain,
                             size_t& outSize) {
  this->insertSignal(frequencyDomain);

  uint8_t ARM_RFFT_FAST_FORWARD = 1U;  // Discrete Inverse Fourier Transform.
  arm_rfft_fast_f32(&rfft_instance, in, out, ARM_RFFT_FAST_FORWARD);

  this->scaleOutput();

  // Copy output data into vector and return.
  outSize = this->numSamples;
  return out;
}

void IFFT::insertSignal(const FrequencyDomain& frequencyDomain) {
  // Copy frequency data into input in the format that CMSIS expects.
  // First element is DC, second is last real value. Then rest is each frequency
  // in order taking up two elements with first being real value and seocnd
  // representing the imaginary value of the complex number.
  // Reference:
  // https://arm-software.github.io/CMSIS-DSP/main/group__RealFFT.html

  in[0] = frequencyDomain.real[0];
  in[1] = frequencyDomain.real[frequencyDomain.N - 1];

  for (int i = 1; i < frequencyDomain.N - 1; i++) {
    in[i * 2] = frequencyDomain.real[i];
    in[i * 2 + 1] = frequencyDomain.img[i];
  }
}

void IFFT::scaleOutput() {
  for (int i = 0; i < numSamples; i++) {
    out[i] /= numSamples;
  }
}
