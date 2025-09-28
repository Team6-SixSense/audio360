#include "signal_processing/window.h"
#include "helper/constants.h"
#include <cmath>

void HannWindow::applyWindow(std::vector<double> &signal) {
  double N = static_cast<double>(signal.size());
  int n = 0;
  for (double &signalValue : signal) {
    double w = std::pow(std::sin(PI * n / (N - 1)), 2.0);
    signalValue *= w;
    n++;
  }
}