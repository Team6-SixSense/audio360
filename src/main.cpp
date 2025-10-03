#ifdef STM_BUILD
#include "hardware_interface/logging/serial_print.h"
#else
#include "features/signal_processing/fft.h"
#endif

int main() {

#ifdef STM_BUILD
  setupLogging();
#else
  std::vector<double> test_vec;
  FFT fft_test(static_cast<uint16_t>(test_vec.size()));
#endif
  while (1) {
    printf("Hello SixSense\n");
  }

  return 0;
}
