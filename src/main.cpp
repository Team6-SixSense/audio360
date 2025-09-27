#include "helper/mp3/mp3.h"
#include "signal_processing/fft.h"
#include <iostream>

int main() {

  std::cout << "Hello SixSense" << std::endl;
  MP3Data data = readMP3File("sample1.mp3");

  std::vector<double> input(data.channel1.begin() + 100000,
                            data.channel1.begin() + 102048);

  FFT fft = FFT(input.size(), input);

  std::vector<double> frequency = fft.signalToFrequency(input);
  std::vector<double> xAxis(frequency.size());

  for (int i = 0; i < frequency.size(); i++) {
    xAxis[i] = (i * 44100) / static_cast<double>(input.size());
    std::cout << "X: " << xAxis[i] << " f: " << frequency[i] << std::endl;
  }

  return 0;
}
