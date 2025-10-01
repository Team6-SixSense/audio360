#pragma once

#include "lib/fftw-3.3.10/api/fftw3.h"
#include "signal_processing/window.h"
#include <vector>

/** @brief Struct for representing FFT output in the frequency domain. */
struct FrequencyDomain {
  /** @brief The number of the data in the FFT output. */
  size_t N;

  /** @brief The frequency (Hz) */
  std::vector<double> frequency;

  /** @brief The real component of the frequency contribution. */
  std::vector<double> real;

  /** @brief The imaginary component of the frequency contribution. */
  std::vector<double> img;

  /** @brief The magnitude of the frequency component. */
  std::vector<double> magnitude;

  /**
   * @brief Construct a new Frequency Domain struct.
   *
   * @param size The number of data points.
   */
  FrequencyDomain(size_t size)
      : N(size), frequency(size), real(size), img(size), magnitude(size) {}
};

/** @brief Fast Fourier Transform (FFT) class. */
class FFT {

public:
  /**
   * @brief Construct a new FFT object.
   *
   * @param inputSize The size of the input signal.
   * @param signal Sample signal of size inputSize. This is used to optimize to
   * find the best FFT algorithm.
   */
  FFT(size_t inputSize, std::vector<double> signal);

  /** @brief Destroy the FFT object. */
  ~FFT();

  /**
   * @brief Converts input signal to the frequency domain.
   *
   * @param signal input signal.
   * @param windowFunction The type of window function to apply to the input
   * signal.
   * @return FrequencyDomain The signal represented in the frequency domain.
   */
  FrequencyDomain
  signalToFrequency(std::vector<double> &signal,
                    WindowFunction windowFunction = WindowFunction::NONE);

private:
  /**
   * @brief Applies the window function on the input signal.
   *
   * @param signal input signal.
   * @param windowFunction Window function.
   */
  void applyWindow(std::vector<double> &signal, WindowFunction windowFunction);

  /**
   * @brief Inserts signal to internal memory of this class.
   *
   * @param signal input signal.
   */
  void insertSignal(std::vector<double> &signal);

  /**
   * @brief Translate FFT algo's output to a portable output to return.
   *
   * @return FrequencyDomain frequency output.
   */
  FrequencyDomain createOutput();

  /** @brief The size of the input signal. */
  size_t inputSize{0};

  /** @brief The size of the output signal. */
  size_t outputSize;

  /** @brief The FFTW plan. This is the algo that is optimized for inputSize
   * signal. */
  fftw_plan plan;

  /** @brief input signal. This memory is shared with fttw_plan plan. */
  double *in;

  /** @brief output signal. This memory is shared with fttw_plan plan. */
  fftw_complex *out;
};
