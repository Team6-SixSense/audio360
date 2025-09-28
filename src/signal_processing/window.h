
#pragma once

#include <vector>

/** @brief Enum for representing the available window functions. */
enum class WindowFunction {
  NONE,
  HANN_WINDOW,
};

/** @brief Abstract class for windowing functions. */
class Window {
public:
  /**
   * @brief Applies a windowing function for the input signal. function applied
   * directly on input.
   *
   * @param signal signal to apply window function on.
   */
  virtual void applyWindow(std::vector<double> &signal) = 0;
};

class HannWindow : public Window {
public:
  void applyWindow(std::vector<double> &signal) override;
};
