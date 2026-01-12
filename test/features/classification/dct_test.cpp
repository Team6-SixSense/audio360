#include "dct.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <vector>
#include <window.hpp>

#include "constants.h"
#include "fft.h"
#include "mel_filter.h"
#include "mp3.h"

static int ArgMax(const std::vector<float>& v) {
  int idx = 0;
  float best = v.empty() ? 0.0f : v[0];
  for (int i = 1; i < static_cast<int>(v.size()); ++i) {
    if (v[i] > best) {
      best = v[i];
      idx = i;
    }
  }
  return idx;
}

// Build a 3-frame STFT-domain by computing 3 FFTs from 3 windows of the MP3.
static std::vector<std::vector<float>> Make3FrameMelSpecFromMP3(
    const MP3Data& data, int sampleRate, int offset0) {
  const uint16_t frameSize = WAVEFORM_SAMPLES;
  const uint16_t numFilters = 40;

  // Extract 3 windows at different offsets.
  std::vector<float> in0(data.channel1.begin() + offset0,
                         data.channel1.begin() + offset0 + frameSize);
  std::vector<float> in1(data.channel1.begin() + offset0 + frameSize,
                         data.channel1.begin() + offset0 + 2 * frameSize);
  std::vector<float> in2(data.channel1.begin() + offset0 + 2 * frameSize,
                         data.channel1.begin() + offset0 + 3 * frameSize);

  FFT fft(frameSize, static_cast<uint16_t>(sampleRate));

  FrequencyDomain fd0 = fft.signalToFrequency(in0, WindowFunction::HANN_WINDOW);
  FrequencyDomain fd1 = fft.signalToFrequency(in1, WindowFunction::HANN_WINDOW);
  FrequencyDomain fd2 = fft.signalToFrequency(in2, WindowFunction::HANN_WINDOW);

  // Stack into STFT-domain object (IMPORTANT: avoid resize() on
  // vector<FrequencyDomain>)
  ShortTimeFourierTransformDomain dom(3);
  dom.stft.reserve(3);
  dom.stft.emplace_back(fd0);
  dom.stft.emplace_back(fd1);
  dom.stft.emplace_back(fd2);

  MelFilter melFilter(numFilters, frameSize, sampleRate);

  std::vector<std::vector<float>> melSpec;
  melFilter.Apply(dom, melSpec);

  return melSpec;
}

TEST(DCTTest, ComputeDCT_OnMelSpectrogram_ProducesExpectedMFCC) {
  // These were validated experiementally using Python to see if relatively the
  // MFCC are obtained.
  std::vector<float> actualFrame1 = {
      -56.664089, 20.234943, 20.820616, 15.136596, 10.813884,
      7.194543,   4.375194,  3.017802,  1.415762,  0.722789,
      -0.017400,  -1.636255, -2.012286};
  std::vector<float> actualFrame2 = {
      -56.810131, 20.132694, 20.845602, 15.574163, 11.382054,
      7.304311,   4.406040,  2.479850,  0.932914,  0.119076,
      0.361818,   -1.280813, -2.317297};
  std::vector<float> actualFrame3 = {
      -56.273273, 20.881832, 21.420116, 15.734683, 10.550714,
      6.176596,   3.137521,  2.407498,  1.427548,  0.890962,
      0.609775,   -0.923622, -2.015059};

  MP3Data data = readMP3File("audio/285_sine.mp3");
  const int offset0 = 100000;  // same style as fft_test.cpp

  const uint16_t numCepstral = 13;
  const uint16_t numFilters = 40;

  DiscreteCosineTransform dct(numCepstral, numFilters);

  std::vector<std::vector<float>> melSpec =
      Make3FrameMelSpecFromMP3(data, SAMPLE_FREQUENCY, offset0);
  std::vector<std::vector<float>> mfccSpec;
  dct.Apply(melSpec, mfccSpec);

  ASSERT_EQ(mfccSpec.size(), 3);
  ASSERT_EQ(mfccSpec[0].size(), numCepstral);

  for (int i = 0; i < numCepstral; ++i) {
    EXPECT_NEAR(mfccSpec[0][i], actualFrame1[i], 1e-3f)
        << "Mismatch at frame 0, coeff " << i;
    EXPECT_NEAR(mfccSpec[1][i], actualFrame2[i], 1e-3f)
        << "Mismatch at frame 1, coeff " << i;
    EXPECT_NEAR(mfccSpec[2][i], actualFrame3[i], 1e-3f)
        << "Mismatch at frame 2, coeff " << i;
  }
}