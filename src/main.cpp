#include <stdio.h>

#ifndef STM_BUILD
#include "features/signal_processing/fft.h"
#endif

int main() {

#ifndef STM_BUILD
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()));
#endif
  printf("Hello SixSense\n");

  return 0;
}
