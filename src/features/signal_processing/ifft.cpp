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

IFFT::IFFT(uint16_t numSamples) : numSamples(numSamples) {
  this->in = new float32_t[this->numSamples]();
  this->out = new float32_t[this->numSamples]();

  this->initializeFFTInstance();
}

IFFT::IFFT(const IFFT& other) : numSamples(other.numSamples) {
  this->in = new float32_t[this->numSamples];
  this->out = new float32_t[this->numSamples];
  std::copy(other.in, other.in + this->numSamples, this->in);
  std::copy(other.out, other.out + this->numSamples, this->out);

  this->initializeFFTInstance();
}

IFFT& IFFT::operator=(const IFFT& other) {
  if (this == &other) {
    return *this;
  }

  // Reallocate if sizes differ
  if (this->numSamples != other.numSamples) {
    delete[] this->in;
    delete[] this->out;

    this->numSamples = other.numSamples;
    this->in = new float32_t[this->numSamples];
    this->out = new float32_t[this->numSamples];
  }

  std::copy(other.in, other.in + this->numSamples, this->in);
  std::copy(other.out, other.out + this->numSamples, this->out);

  this->initializeFFTInstance();

  return *this;
}

IFFT::~IFFT() {
  // Free memory.
  delete[] this->in;
  delete[] this->out;
}

std::vector<float> IFFT::frequencyToTime(
    const FrequencyDomain& frequencyDomain) {
  this->insertSignal(frequencyDomain);

  uint8_t ARM_RFFT_FAST_FORWARD = 1U;  // Discrete Inverse Fourier Transform.
  arm_rfft_fast_f32(&rfft_instance, this->in, this->out, ARM_RFFT_FAST_FORWARD);

  this->scaleOutput();

  // Copy output data into vector and return.
  return std::vector<float>(this->out, this->out + numSamples);
}

void IFFT::insertSignal(const FrequencyDomain& frequencyDomain) {
  assert(this->numSamples == (frequencyDomain.N - 1) * 2);

  // Copy frequency data into input in the format that CMSIS expects.
  // First element is DC, second is last real value. Then rest is each frequency
  // in order taking up two elements with first being real value and seocnd
  // representing the imaginary value of the complex number.
  // Reference:
  // https://arm-software.github.io/CMSIS-DSP/main/group__RealFFT.html

  this->in[0] = frequencyDomain.real[0];
  this->in[1] = frequencyDomain.real[frequencyDomain.N - 1];

  for (int i = 1; i < frequencyDomain.N - 1; i++) {
    this->in[i * 2] = frequencyDomain.real[i];
    this->in[i * 2 + 1] = frequencyDomain.img[i];
  }
}

void IFFT::scaleOutput() {
  for (int i = 0; i < numSamples; i++) {
    this->out[i] /= numSamples;
  }
}
