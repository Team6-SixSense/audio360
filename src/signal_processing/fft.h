#pragma once

#include "lib\fftw-3.3.10\api\fftw3.h"
#include <vector>

// TODO: add docstring to the function.

class FFT {

public:
  FFT(size_t inputSize, std::vector<double> signal);

  ~FFT();

  std::vector<double> signalToFrequency(std::vector<double> &signal);

private:
  void insertSignal(std::vector<double> &signal);

  std::vector<double> createOutput();

  size_t inputSize{0};

  size_t outputSize;

  fftw_plan plan;

  double *in;

  fftw_complex *out;
};
