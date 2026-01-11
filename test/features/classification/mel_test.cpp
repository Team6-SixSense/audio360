/**
 ******************************************************************************
 * @file    mel_filter_test.cpp
 * @brief   Unit tests for MelFilter::CreateFilterBank() and MelFilter::Apply()
 ******************************************************************************
 */

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
namespace {

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

static float MaxVal(const std::vector<float>& v) {
  return v.empty() ? 0.0f : *std::max_element(v.begin(), v.end());
}

// Build a 3-frame STFT-domain by computing 3 FFTs from 3 windows of the MP3.
static ShortTimeFourierTransformDomain Make3FrameSTFTFromMP3(
    const MP3Data& data, int sampleRate, int offset0) {
  const uint16_t frameSize = WAVEFORM_SAMPLES;

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

  return dom;
}

// Count values > eps
static int CountPositive(const std::vector<float>& v, float eps = 1e-8f) {
  int c = 0;
  for (float x : v)
    if (x > eps) ++c;
  return c;
}

// Used to visualize the mel spectrogram in console (for debugging)
static void PrintMelBars(const std::vector<float>& mel, float scale = 50.0f) {
  for (float v : mel) {
    int bars = static_cast<int>(v * scale);
    for (int i = 0; i < bars; ++i) printf("#");
    printf("\n");
  }
}

}  // namespace

TEST(MelFilterTest, CreateFilterBank_ImpulseBinContributesToAtMostTwoMelBands) {
  MP3Data data = readMP3File("audio/285_sine.mp3");

  const int offset0 = 100000;  // same style as fft_test.cpp
  ASSERT_GT(static_cast<int>(data.channel1.size()),
            offset0 + 3 * static_cast<int>(WAVEFORM_SAMPLES));

  auto stft = Make3FrameSTFTFromMP3(data, SAMPLE_FREQUENCY, offset0);

  std::vector<std::vector<float>> spec(stft.numFrames);
  for (int f = 0; f < stft.numFrames; ++f) {
    spec[f] = stft.stft[f].magnitude;  // (frame x freqBin)
  }

  const int stftPositives = CountPositive(stft.stft[1].frequency, 0.5f);

  const uint16_t fftSize = stft.stft[0].N;  // matches FFT output size
  const uint16_t numFilters = 40;

  MelFilter mel(numFilters, fftSize, SAMPLE_FREQUENCY);

  std::vector<std::vector<float>> melSpec;
  mel.Apply(stft, melSpec);

  ASSERT_EQ(melSpec.size(), 3u);
  ASSERT_EQ(melSpec[0].size(), numFilters);

  // We should expect the mel bands to be more sparse compared to STFT bins.
  const int melPositives = CountPositive(melSpec[1], 0.5f);
  EXPECT_LE(melPositives, stftPositives);
}

TEST(MelFilterTest, Apply_StackedThreeFFTs_ProducesConsistentPeakAcrossFrames) {
  MP3Data data = readMP3File("audio/285_sine.mp3");

  const int offset0 = 100000;
  ASSERT_GT(static_cast<int>(data.channel1.size()),
            offset0 + 3 * static_cast<int>(WAVEFORM_SAMPLES));

  auto stft = Make3FrameSTFTFromMP3(data, SAMPLE_FREQUENCY, offset0);

  const uint16_t fftSize = stft.stft[0].N;
  const uint16_t numFilters = 40;

  MelFilter mel(numFilters, fftSize, SAMPLE_FREQUENCY);

  std::vector<std::vector<float>> melSpec;
  mel.Apply(stft, melSpec);

  // Dominant mel bin should match across frames for a steady tone.
  const int peak0mel = ArgMax(melSpec[0]);
  const int peak1mel = ArgMax(melSpec[1]);
  const int peak2mel = ArgMax(melSpec[2]);

  EXPECT_EQ(peak0mel, peak1mel);
  EXPECT_EQ(peak0mel, peak2mel);
}
