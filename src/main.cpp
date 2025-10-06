#ifdef STM_BUILD
#include "hardware_interface/logging/serial_print.h"
#else
#include "features/signal_processing/fft.h"
#endif

#include <stdio.h>

int main() {

#ifdef STM_BUILD
  setupLogging();
#else
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()));
#endif
  int i = 0;
  while (1) {
    printf("Hello SixSense\n");
    i++;
    printf("Hello data\n");
    i++;
    printf("Hello data1\n");
    i++;
    printf("Hello data2\n");
    i++;
    printf("Hello data3\n");
    i++;
  }

  return 0;
}
