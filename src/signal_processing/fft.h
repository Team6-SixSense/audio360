#include <fftw3.h>
#include <vector>

// TODO: add docstring to the function.

class FFT {

  FFT(size_t inputSize);

  ~FFT();

  std::vector<double> signalToFrequency(std::vector<double> signal);

  void insertSignal(std::vector<double> signal);

  std::vector<double> createOutput();

  size_t inputSize{0};

  size_t outputSize;

  fftw_plan plan;

  fftw_complex *in;

  fftw_complex *out;
};
