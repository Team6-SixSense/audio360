
#include "signal_processing/fft.h"
#include "helper/constants.h"
#include <cmath>
#include <stdio.h>

FFT::FFT(uint16_t inputSize)
    : inputSize(inputSize), outputSize(inputSize / 2 + 1) {

  this->in = new float[inputSize];
  this->out = new float[inputSize];

  // Initialize the FFT instance when using CMSIS DSP library.
  arm_status status = arm_rfft_fast_init_f32(&rfft_instance, inputSize);

  if (status != arm_status::ARM_MATH_SUCCESS) {
    printf("[ERROR] Error in initializing CMSIS DSP FFT. Error status code %d",
           status);
  }
}

FFT::~FFT() {

  // Free memory.
  delete[] this->in;
  delete[] this->out;
}

FrequencyDomain FFT::signalToFrequency(std::vector<float> &signal,
                                       WindowFunction windowFunction) {
  // TODO: check that the signal size is equal to the inputsize

  this->applyWindow(signal, windowFunction);
  this->insertSignal(signal);

  uint8_t flag = {1U}; // Discrete Fourier Transform.
  arm_rfft_fast_f32(&rfft_instance, this->in, this->out, flag);

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

  FrequencyDomain frequencyDomain(this->outputSize);

  // TODO: Potential to vectorize this calculation. Either with eigen or via
  // hardware.
  for (int i = 0; i < this->outputSize; i++) {
    double real = this->out[2 * i];
    double img = this->out[2 * i + 1];

    frequencyDomain.frequency[i] = (i * SAMPLE_FREQUENCY) / this->inputSize;
    frequencyDomain.real[i] = real;
    frequencyDomain.img[i] = img;
    frequencyDomain.magnitude[i] = std::sqrt(real * real + img * img);
  }
  return frequencyDomain;
}