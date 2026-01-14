/**
 ******************************************************************************
 * @file    fft.h
 * @brief   Fast Fourier Transform (FFT) header
 ******************************************************************************
 */

#pragma once

#include "arm_math.h"

#ifdef STM_BUILD
// stm32f767xx include must be first include to use CMSIS library.
#include "stm32f767xx.h"

#endif
#include <cmath>
#include <vector>

#include "frequencyDomain.h"
#include "window.hpp"

/** @brief Fast Fourier Transform (FFT) class. */
class FFT {
 public:
  /**
   * @brief Construct a new FFT object.
   *
   * @param inputSize The size of the input signal.
   * @param sampleFrequency The sample frequency.
   */
  FFT(uint16_t inputSize, int sampleFrequency);

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
  FrequencyDomain signalToFrequency(
      std::vector<float>& signal,
      WindowFunction windowFunction = WindowFunction::NONE);

 private:
  /**
   * @brief Applies the window function on the input signal.
   *
   * @param signal input signal.
   * @param windowFunction Window function.
   */
  void applyWindow(std::vector<float>& signal, WindowFunction windowFunction);

  /**
   * @brief Inserts signal to internal memory of this class.
   *
   * @param signal input signal.
   */
  void insertSignal(std::vector<float>& signal);

  inline void insertFrequencyEntry(FrequencyDomain& frequencyDomain, size_t pos,
                                   float frequency, float real, float img) {
    frequencyDomain.frequency[pos] = frequency;
    frequencyDomain.real[pos] = real;
    frequencyDomain.img[pos] = img;
    frequencyDomain.magnitude[pos] = std::sqrt(real * real + img * img);
    frequencyDomain.powerMagnitude[pos] =
        frequencyDomain.magnitude[pos] * frequencyDomain.magnitude[pos];
  }

  /**
   * @brief Translate FFT algo's output to a portable output to return.
   *
   * @return FrequencyDomain frequency output.
   */
  FrequencyDomain createOutput();

  /** @brief The size of the input signal. */
  uint16_t inputSize{0U};

  /** @brief The sample frequency. */
  int sampleFrequency;

  /** @brief The size of the output signal. */
  uint16_t outputSize;

  /** @brief input signal. This memory is shared with fttw_plan plan. */
  float32_t* in;

  /** @brief output signal. This memory is shared with fttw_plan plan. */
  float32_t* out;

  /** @brief Real FFT instance for using CMSIS DSP library */
  arm_rfft_fast_instance_f32 rfft_instance;
};
