/**
 ******************************************************************************
 * @file    fft_test.cpp
 * @brief   Unit tests for FFT (Fast Fourier Transform). Tests here uses a mock
 *          as hardware acceleration method are not available for CI runs.
 ******************************************************************************
 */

#include "fft.h"

#include <gtest/gtest.h>

#include "constants.h"
#include "mp3.h"

class FFTTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Read in test mp3 data.
    MP3Data data = readMP3File("audio/285_sine.mp3");

    // Choose an arbitray window in the middle of the mp3.
    const int OFFSET = 100000;
    input.resize(WAVEFORM_SAMPLES);
    std::copy(data.channel1.begin() + OFFSET,
              data.channel1.begin() + OFFSET + WAVEFORM_SAMPLES, input.begin());
  }

  std::vector<float> input{};
};

/** @brief Given a 285 Hz sine audio signal, assert that the FFT has the largest
 * magnitude around 285 Hz. */
TEST_F(FFTTest, SignalToFrequency_285Hz) {
  // Run FFT.
  FFT fft = FFT(static_cast<uint16_t>(input.size()), 44100);
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

/** @brief Given a 285 Hz sine audio signal, assert that the FFT has the largest
 * magnitude around 285 Hz. Verify on copy FFT. */
TEST_F(FFTTest, SignalToFrequencyCopy_285Hz) {
  // Run FFT.
  FFT fft = FFT(static_cast<uint16_t>(input.size()), 44100);
  FFT copyFft(fft);
  FrequencyDomain frequencyDomain =
      copyFft.signalToFrequency(input, WindowFunction::HANN_WINDOW);

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

/** @brief Given a 285 Hz sine audio signal, assert that the FFT has the largest
 * magnitude around 285 Hz. Verify on assignment FFT. */
TEST_F(FFTTest, SignalToFrequencyAssignment_285Hz) {
  // Run FFT.
  FFT fft = FFT(static_cast<uint16_t>(input.size()), 44100);
  FFT assignmentFft = FFT(static_cast<uint16_t>(input.size() + 1),
                          44100);  // Plus one to test size re-adjustment.
  assignmentFft = fft;
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
