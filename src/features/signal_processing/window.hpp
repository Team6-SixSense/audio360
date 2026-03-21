/**
 ******************************************************************************
 * @file    window.hpp
 * @brief   Frequency window functions.
 ******************************************************************************
 */

#pragma once

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
  virtual void applyWindow(T* signal, uint32_t size) = 0;
};

template <typename T>
class HannWindow : public Window<T> {
 public:
  void applyWindow(T* signal, uint32_t size) override {
    for (size_t i = 0; i < size; i++) {
      T w = static_cast<T>(std::pow(std::sin(PI_32 * i / (size - 1.0f)), 2.0));
      signal[i] *= w;
    }
  }
};
