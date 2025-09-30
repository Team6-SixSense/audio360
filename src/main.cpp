#include <stdio.h>

#ifndef STM_BUILD
#include "signal_processing/fft.h"
#endif

int main() {

#ifndef STM_BUILD
  std::vector<double> test_vec;
  FFT fft_test(test_vec.size(), test_vec);
#endif
  printf("Hello SixSense\n");

  return 0;
}
