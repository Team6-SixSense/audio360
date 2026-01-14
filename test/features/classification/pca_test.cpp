#include "pca.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <vector>
#include <window.hpp>

#include "constants.h"
#include "dct.h"
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
static std::vector<std::vector<float>> Make3FrameMFCCSpecFromMP3(
    const MP3Data& data, int sampleRate, int offset0) {
  const uint16_t frameSize = WAVEFORM_SAMPLES;
  const uint16_t numFilters = 40;
  const uint16_t numCepstral = 13;

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

  DiscreteCosineTransform dct(numCepstral, numFilters);
  std::vector<std::vector<float>> mfccSpec;
  dct.Apply(melSpec, mfccSpec);

  return mfccSpec;
}

TEST(PCA, ApplyPCA) {
  // Load MP3 data from file.
  MP3Data data = readMP3File("audio/285_sine.mp3");
  const int offset0 = 100000;
  const uint16_t numCepstral = 13;
  const uint16_t numPCAComponents = 13;

  // Apply PCA to the MFCC feature vector.
  PrincipleComponentAnalysis pca(numPCAComponents, numCepstral);

  // Create MFCC feature vector from MP3 data.
  std::vector<std::vector<float>> mfccFeatureVector =
      Make3FrameMFCCSpecFromMP3(data, SAMPLE_FREQUENCY, offset0);

  // printf("MFCC Feature Vector:\n");
  // for (const auto& frame : mfccFeatureVector) {
  //   for (const auto& value : frame) {
  //     printf("%f ", value);
  //   }
  //   printf("\n");
  // }

  std::vector<std::vector<float>> pcaFeatureVector;
  pca.Apply(mfccFeatureVector, pcaFeatureVector);

  // Visualize the PCA feature vector.

  // printf("PCA Feature Vector:\n");
  // for (const auto& frame : pcaFeatureVector) {
  //   for (const auto& value : frame) {
  //     printf("%f ", value);
  //   }
  //   printf("\n");
  // }

  // Verify the size of the output PCA feature vector.
  ASSERT_EQ(pcaFeatureVector.size(), mfccFeatureVector.size());
  for (const auto& frame : pcaFeatureVector) {
    ASSERT_EQ(frame.size(), numPCAComponents);
  }
}
