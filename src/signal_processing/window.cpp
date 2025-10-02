#include "signal_processing/window.h"
#include "helper/constants.h"
#include <cmath>

template <typename T> void HannWindow<T>::applyWindow(std::vector<T> &signal) {
  T N = static_cast<T>(signal.size());
  int n = 0;
  for (T &signalValue : signal) {
    T w = std::pow(std::sin(PI_32 * n / (N - 1)), 2.0);
    signalValue *= w;
    n++;
  }
}