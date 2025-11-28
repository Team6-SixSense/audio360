/**
 ******************************************************************************
 * @file    fft.cpp
 * @brief   Fast Fourier Transform (FFT) class
 ******************************************************************************
 */

#include "features/signal_processing/fft.h"

#include <stdio.h>

#include <algorithm>
#include <cassert>
#include <cmath>

#include "helper/constants.h"
#include "helper/logging/logging.hpp"

FFT::FFT(uint16_t inputSize, int sampleFrequency)
    : inputSize(inputSize), sampleFrequency(sampleFrequency) {
  this->in = new float32_t[this->inputSize]();
  this->outputSize = this->inputSize;
  this->out = new float32_t[this->outputSize]();

  // Initialize the FFT instance when using CMSIS DSP library.
  arm_status status = arm_rfft_fast_init_f32(&rfft_instance, inputSize);

  if (status != arm_status::ARM_MATH_SUCCESS) {
    ERROR("Error in initializing CMSIS DSP FFT. Error status code %d", status);
  }
}

FFT::~FFT() {
  // Free memory.
  delete[] this->in;
  delete[] this->out;
}

FrequencyDomain FFT::signalToFrequency(std::vector<float>& signal,
                                       WindowFunction windowFunction) {
  this->applyWindow(signal, windowFunction);
  this->insertSignal(signal);

  uint8_t ARM_RFFT_FAST_FORWARD = 0U;  // Discrete Fourier Transform.
  arm_rfft_fast_f32(&rfft_instance, this->in, this->out, ARM_RFFT_FAST_FORWARD);

  return this->createOutput();
}

void FFT::applyWindow(std::vector<float>& signal,
                      WindowFunction windowFunction) {
  switch (windowFunction) {
    case WindowFunction::NONE:
      INFO("No windowing function is applied.");
      break;
    case WindowFunction::HANN_WINDOW:
      HannWindow<float>().applyWindow(signal);
      break;
    default:
      WARN("Window function is not supported. Signal remain the same.");
  }
}

void FFT::insertSignal(std::vector<float>& signal) {
  assert(signal.size() == inputSize);
  std::copy(signal.begin(), signal.end(), this->in);
}

FrequencyDomain FFT::createOutput() {
  uint16_t N = (this->inputSize / 2) + 1;
  uint16_t lastIdx = N - 1;
  FrequencyDomain frequencyDomain(N);

  for (int i = 0; i < N; i++) {
    // Since first FFT output is DC, there is no imaginary part. Thus CMSIS-DSP
    // library stores the last real value in the place of the first complex
    // value. Reference:
    // https://arm-software.github.io/CMSIS-DSP/main/group__RealFFT.html

    float real;
    float img;

    if (i != 0 && i != lastIdx) {
      real = this->out[2 * i];
      img = this->out[2 * i + 1];
    } else {
      int idx = (i == lastIdx) ? 1 : 0;  // idx takes 0 when i = 0.
      real = this->out[idx];
      img = 0.0f;
    }

    frequencyDomain.frequency[i] =
        (i * this->sampleFrequency) / static_cast<float>(this->inputSize);
    frequencyDomain.real[i] = real;
    frequencyDomain.img[i] = img;
    frequencyDomain.magnitude[i] = std::sqrt(real * real + img * img);
  }
  return frequencyDomain;
}