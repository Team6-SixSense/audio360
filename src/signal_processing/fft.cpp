
#include "signal_processing/fft.h"

/**
 * Implementation for using fftw library provided by their documentation;
 * Section 2.1. and 2.3. https://www.fftw.org/fftw3.pdf
 */

FFT::FFT(size_t inputSize, std::vector<double> signal)
    : inputSize(inputSize), outputSize(inputSize / 2 + 1) {

  // Create a plan. This will optimize and find the best algo to run based on
  // the size of the signal and optimizer type.
  this->in = (double *)fftw_malloc(sizeof(double) * this->inputSize);
  this->insertSignal(signal); // Need a reference signal for optimization.
  this->out = (double *)fftw_malloc(sizeof(fftw_complex) * this->outputSize);
  this->plan = fftw_plan_dft_r2c_1d(this->inputSize, in, out, FFTW_ESTIMATE);
}

FFT::~FFT() {

  // Free memory.
  fftw_destroy_plan(plan);
  fftw_free(in);
  fftw_free(out);
}

std::vector<double> FFT::signalToFrequency(std::vector<double> signal) {

  this->insertSignal(signal);
  fftw_execute(plan);
  return this->createOutput();
}

void FFT::insertSignal(std::vector<double> signal) {
  std::copy(signal.begin(), signal.end(), in);
}

std::vector<double> FTT::createOutput() {
  std::vector<double> frequency(this->outputSize);
  std::copy(std::begin(out), std::end(out), frequency.begin());
  return frequency;
}