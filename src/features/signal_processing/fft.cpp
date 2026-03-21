/**
 ******************************************************************************
 * @file    fft.cpp
 * @brief   Fast Fourier Transform (FFT) class
 ******************************************************************************
 */

#include "fft.h"

#include <stdio.h>

#include <algorithm>
#include <cassert>
#include <cmath>

#include "constants.h"

/** @brief input signal. This memory is declared in fft.h. */
float32_t FFT::in[FFT_BUFFER_SIZE_IN] = {0.0f};

/** @brief output signal. This memory is declared in fft.h. */
float32_t FFT::out[FFT_BUFFER_SIZE_OUT] = {0.0f};

FFT::FFT(uint16_t inputSize, int sampleFrequency)
    : inputSize(inputSize),
      sampleFrequency(sampleFrequency),
      outputSize(inputSize) {
  this->initializeFFTInstance();
}

FFT::FFT(const FFT& other)
    : inputSize(other.inputSize),
      sampleFrequency(other.sampleFrequency),
      outputSize(other.outputSize) {
  this->initializeFFTInstance();
}

FFT::~FFT() = default;

void FFT::signalToFrequency(
    float* signal, FrequencyDomain& outFreq,
    WindowFunction windowFunction = WindowFunction::NONE) {
  this->insertSignal(signal);
  this->applyWindow(in, windowFunction);  // dont modify input buffer.

  uint8_t ARM_RFFT_FAST_FORWARD = 0U;  // Discrete Fourier Transform.
  arm_rfft_fast_f32(&rfft_instance, in, out, ARM_RFFT_FAST_FORWARD);

  this->createOutput(outFreq);
}

void FFT::applyWindow(float* signal, WindowFunction windowFunction) {
  switch (windowFunction) {
    case WindowFunction::NONE:
      INFO("No windowing function is applied.");
      break;
    case WindowFunction::HANN_WINDOW:
      HannWindow<float>().applyWindow(signal, inputSize);
      break;
    default:
      WARN("Window function is not supported. Signal remain the same.");
  }
}

void FFT::insertSignal(float* signal) const {
  std::copy(signal, signal + inputSize, in);
}

void FFT::createOutput(FrequencyDomain& outFreq) {
  uint16_t N = (this->inputSize / 2) + 1;
  uint16_t lastIdx = N - 1;
  float frequency = 0.0f;
  float frequencyBinSize =
      this->sampleFrequency / static_cast<float>(this->inputSize);

  // Since first FFT output is DC, there is no imaginary part. Thus CMSIS-DSP
  // library stores the last real value in the place of the first complex value.
  // Reference:
  // https://arm-software.github.io/CMSIS-DSP/main/group__RealFFT.html

  // DC.
  insertFrequencyEntry(outFreq, 0, frequency, out[0], 0.0f);
  frequency += frequencyBinSize;

  // Middle real and complex values.
  for (int i = 1; i < lastIdx; i++) {
    insertFrequencyEntry(outFreq, i, frequency, out[2 * i], out[2 * i + 1]);
    frequency += frequencyBinSize;
  }

  // Last real.
  insertFrequencyEntry(outFreq, lastIdx, frequency, out[1], 0.0f);
}
