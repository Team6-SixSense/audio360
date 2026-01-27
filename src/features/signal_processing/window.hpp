/**
 ******************************************************************************
 * @file    window.hpp
 * @brief   Frequency window functions.
 ******************************************************************************
 */

#pragma once

#include <vector>

#include "constants.h"


/** @brief Enum for representing the available window functions. */
enum class WindowFunction { NONE, HANN_WINDOW };

/** @brief Abstract class for windowing functions. */
template <typename T>
class Window {
 public:
  /** @brief Destroy the Window object */
  virtual ~Window() = default;

  /**
   * @brief Applies a windowing function for the input signal. function applied
   * directly on input.
   *
   * @param signal signal to apply window function on.
   */
  virtual void applyWindow(std::vector<T>& signal) = 0;
};

template <typename T>
class HannWindow : public Window<T> {
 public:
  void applyWindow(std::vector<T>& signal) override {
    T N = static_cast<T>(signal.size());
    int n = 0;
    for (T& signalValue : signal) {
      T w = static_cast<T>(std::pow(std::sin(PI_32 * n / (N - 1)), 2.0));
      signalValue *= w;
      n++;
    }
  }
};
