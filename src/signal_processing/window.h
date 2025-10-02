
#pragma once

#include <vector>

/** @brief Enum for representing the available window functions. */
enum class WindowFunction { NONE, HANN_WINDOW };

/** @brief Abstract class for windowing functions. */
template <typename T> class Window {
public:
  /** @brief Destroy the Window object */
  virtual ~Window() = default;

  /**
   * @brief Applies a windowing function for the input signal. function applied
   * directly on input.
   *
   * @param signal signal to apply window function on.
   */
  virtual void applyWindow(std::vector<T> &signal) = 0;
};

template <typename T> class HannWindow : public Window<T> {
public:
  void applyWindow(std::vector<T> &signal) override;
};
