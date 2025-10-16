/**
 ******************************************************************************
 * @file    fft.cpp
 * @brief   Fast Fourier Transform (FFT) class
 ******************************************************************************
 */

#include "features/signal_processing/fft.h"
#include "helper/constants.h"
#include <algorithm>
#include <cmath>
#include <stdio.h>

FFT::FFT(uint16_t inputSize) : inputSize(inputSize) {
  this->in = new float[inputSize];

#ifdef STM_BUILD
  this->outputSize = this->inputSize;
  this->out = new float[this->outputSize];

  // Initialize the FFT instance when using CMSIS DSP library.
  arm_status status = arm_rfft_fast_init_f32(&rfft_instance, inputSize);

  if (status != arm_status::ARM_MATH_SUCCESS) {
    printf("[ERROR] Error in initializing CMSIS DSP FFT. Error status code %d",
           status);
  }
#else
  // Simple FFT uses complex input only. Thus output is size N. However due to
  // Hermitian symmetry, and input signal in our case being real values, we only
  // care about the first half.
  this->outputSize = (this->inputSize / 2) + 1;
  this->complexOutput.resize(this->inputSize);

#endif
}

FFT::~FFT() {

#ifdef STM_BUILD
  // Free memory.
  delete[] this->in;
  delete[] this->out;
#endif
}

FrequencyDomain FFT::signalToFrequency(std::vector<float> &signal,
                                       WindowFunction windowFunction) {
  // TODO: check that the signal size is equal to the inputsize

  this->applyWindow(signal, windowFunction);
  this->insertSignal(signal);

#ifdef STM_BUILD
  uint8_t flag = {1U}; // Discrete Fourier Transform.
  arm_rfft_fast_f32(&rfft_instance, this->in, this->out, flag);
#else
  const char *error = NULL; // error description
  simple_fft::FFT(this->in, this->complexOutput, this->inputSize, error);
#endif

  return this->createOutput();
}

void FFT::applyWindow(std::vector<float> &signal,
                      WindowFunction windowFunction) {
  switch (windowFunction) {
  case WindowFunction::NONE:
    printf("[INFO] No windowing function is applied.");
    break;
  case WindowFunction::HANN_WINDOW:
    HannWindow<float>().applyWindow(signal);
    break;
  default:
    printf("[WARN] Window function is not supported. Signal remain the same.");
  }
}

void FFT::insertSignal(std::vector<float> &signal) {
  std::copy(signal.begin(), signal.end(), this->in);
}

FrequencyDomain FFT::createOutput() {

  uint16_t N = (this->inputSize / 2) + 1;
  uint16_t lastIdx = N - 1;
  FrequencyDomain frequencyDomain(N);

  for (int i = 0; i < this->outputSize; i++) {
#ifdef STM_BUILD
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
      int idx = (i == lastIdx) ? 1 : 0; // idx takes 0 when i = 0.
      real = this->out[idx];
      img = 0.0f;
    }

#else
    float real = static_cast<float>(this->complexOutput[i].real());
    float img = static_cast<float>(this->complexOutput[i].imag());
#endif

    frequencyDomain.frequency[i] =
        (i * SAMPLE_FREQUENCY) / static_cast<float>(this->inputSize);
    frequencyDomain.real[i] = real;
    frequencyDomain.img[i] = img;
    frequencyDomain.magnitude[i] = std::sqrt(real * real + img * img);
  }
  return frequencyDomain;
}