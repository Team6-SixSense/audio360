#pragma once

#ifdef STM_BUILD
// stm32f767xx include must be first include to use CMSIS library.
#include "stm32f767xx.h"

#include "arm_math.h"
#else
#define __USE_SQUARE_BRACKETS_FOR_ELEMENT_ACCESS_OPERATOR // Used by simple FFT
#include "lib/simple_fft/include/simple_fft/fft.hpp"
#include <complex>
#endif

#include "features/signal_processing/window.hpp"
#include <vector>

/** @brief Struct for representing FFT output in the frequency domain. */
struct FrequencyDomain {
  /** @brief The number of the data in the FFT output. */
  uint16_t N;

  /** @brief The frequency (Hz) */
  std::vector<float> frequency;

  /** @brief The real component of the frequency contribution. */
  std::vector<float> real;

  /** @brief The imaginary component of the frequency contribution. */
  std::vector<float> img;

  /** @brief The magnitude of the frequency component. */
  std::vector<float> magnitude;

  /**
   * @brief Construct a new Frequency Domain struct.
   *
   * @param size The number of data points.
   */
  FrequencyDomain(uint16_t size)
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
  FFT(uint16_t inputSize);

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
  signalToFrequency(std::vector<float> &signal,
                    WindowFunction windowFunction = WindowFunction::NONE);

private:
  /**
   * @brief Applies the window function on the input signal.
   *
   * @param signal input signal.
   * @param windowFunction Window function.
   */
  void applyWindow(std::vector<float> &signal, WindowFunction windowFunction);

  /**
   * @brief Inserts signal to internal memory of this class.
   *
   * @param signal input signal.
   */
  void insertSignal(std::vector<float> &signal);

  /**
   * @brief Translate FFT algo's output to a portable output to return.
   *
   * @return FrequencyDomain frequency output.
   */
  FrequencyDomain createOutput();

  /** @brief The size of the input signal. */
  uint16_t inputSize{0U};

  /** @brief The size of the output signal. */
  uint16_t outputSize;

  /** @brief input signal. This memory is shared with fttw_plan plan. */
  float *in;

  /** @brief output signal. This memory is shared with fttw_plan plan. */
  float *out;

#ifdef STM_BUILD
  /** @brief Real FFT instance for using CMSIS DSP library */
  arm_rfft_fast_instance_f32 rfft_instance;
#else
  /** @brief Complex output vector representing frequency domain when using the
   * Simple-FFT library. */
  std::vector<std::complex<double>> complexOutput;
#endif
};
