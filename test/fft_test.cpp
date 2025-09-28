#include "helper/constants.h"
#include "helper/mp3/mp3.h"
#include "signal_processing/fft.h"
#include <gtest/gtest.h>

/** @brief Given a 285 Hz sine audio signal, assert that the FFT has the largest
 * magnitude around 285 Hz. */
TEST(FFTTest, SignalToFrequency_285Hz) {
  // Read in test mp3 data.
  MP3Data data = readMP3File("audio/285_sine.mp3");

  // Choose an arbitray window in the middle of the mp3.
  const int OFFSET = 100000;
  std::vector<double> input(data.channel1.begin() + OFFSET,
                            data.channel1.begin() + OFFSET + WINDOW_SIZE);

  // Run  FFT.
  FFT fft = FFT(input.size(), input);
  FrequencyDomain frequencyDomain =
      fft.signalToFrequency(input, WindowFunction::HANN_WINDOW);

  // Assert that frequency closest to 285Hz has the largest magnitude.
  double maxMagnitude{0.0};
  int maxMagnitudeIdx = 0;
  for (int i = 0; i < frequencyDomain.N; i++) {
    if (frequencyDomain.magnitude[i] > maxMagnitude) {
      maxMagnitude = frequencyDomain.magnitude[i];
      maxMagnitudeIdx = i;
    }
  }

  double frequencyMaxMagnitude = frequencyDomain.frequency[maxMagnitudeIdx];
  double windowBinsize =
      SAMPLE_FREQUENCY / static_cast<double>(frequencyDomain.N);

  EXPECT_NEAR(285.0, frequencyMaxMagnitude, windowBinsize);
}
