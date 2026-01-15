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

FFT::FFT(uint16_t inputSize, int sampleFrequency)
    : inputSize(inputSize),
      sampleFrequency(sampleFrequency),
      outputSize(inputSize) {
  this->in = new float32_t[this->inputSize]();
  this->out = new float32_t[this->outputSize]();

  this->initializeFFTInstance();
}

FFT::FFT(const FFT& other)
    : inputSize(other.inputSize),
      sampleFrequency(other.sampleFrequency),
      outputSize(other.sampleFrequency) {
  this->in = new float32_t[this->inputSize];
  this->out = new float32_t[this->outputSize];
  std::copy(other.in, other.in + this->inputSize, this->in);
  std::copy(other.out, other.out + this->outputSize, this->out);

  this->initializeFFTInstance();
}

FFT& FFT::operator=(const FFT& other) {
  if (this == &other) {
    return *this;
  }

  // Reallocate if sizes differ
  if (this->inputSize != other.inputSize) {
    delete[] this->in;
    delete[] this->out;

    this->inputSize = other.inputSize;
    this->outputSize = other.outputSize;
    this->in = new float32_t[this->inputSize];
    this->out = new float32_t[this->outputSize];
  }

  this->sampleFrequency = other.sampleFrequency;

  std::copy(other.in, other.in + this->inputSize, this->in);
  std::copy(other.out, other.out + this->outputSize, this->out);

  this->initializeFFTInstance();
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
  float frequency = 0.0f;
  float frequencyBinSize =
      this->sampleFrequency / static_cast<float>(this->inputSize);
  float real;
  float img;

  // Since first FFT output is DC, there is no imaginary part. Thus CMSIS-DSP
  // library stores the last real value in the place of the first complex value.
  // Reference:
  // https://arm-software.github.io/CMSIS-DSP/main/group__RealFFT.html

  // DC.
  insertFrequencyEntry(frequencyDomain, 0, frequency, this->out[0], 0.0f);
  frequency += frequencyBinSize;

  // Middle real and complex values.
  for (int i = 1; i < lastIdx; i++) {
    insertFrequencyEntry(frequencyDomain, i, frequency, this->out[2 * i],
                         this->out[2 * i + 1]);
    frequency += frequencyBinSize;
  }

  // Last real.
  insertFrequencyEntry(frequencyDomain, lastIdx, frequency, this->out[1], 0.0f);

  return frequencyDomain;
}
