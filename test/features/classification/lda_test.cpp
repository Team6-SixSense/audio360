#include "lda.h"

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
#include "pca.h"

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
static std::vector<std::vector<float>> Make3FramePCASpecFromMP3(
    const MP3Data& data, int sampleRate, int offset0) {
  const uint16_t frameSize = WAVEFORM_SAMPLES;
  const uint16_t numFilters = 40;
  const uint16_t numCepstral = 13;
  const uint16_t numPCAComponents = 13;

  const uint16_t numFrames = data.channel1.size() / frameSize;

  FFT fft(static_cast<uint16_t>(frameSize), static_cast<uint16_t>(sampleRate));

  ShortTimeFourierTransformDomain dom(numFrames);

  dom.stft.reserve(numFrames);

  for (size_t frame = 0; frame < numFrames; ++frame) {
    std::vector<float> in(data.channel1.begin() + frame * frameSize,
                          data.channel1.begin() + (frame + 1) * frameSize);
    FrequencyDomain fd = fft.signalToFrequency(in, WindowFunction::HANN_WINDOW);
    dom.stft.emplace_back(fd);
  }

  // // Extract 3 windows at different offsets.
  // std::vector<float> in0(data.channel1.begin() + offset0,
  //                        data.channel1.begin() + offset0 + frameSize);
  // std::vector<float> in1(data.channel1.begin() + offset0 + frameSize,
  //                        data.channel1.begin() + offset0 + 2 * frameSize);
  // std::vector<float> in2(data.channel1.begin() + offset0 + 2 * frameSize,
  //                        data.channel1.begin() + offset0 + 3 * frameSize);

  // FFT fft(static_cast<uint16_t>(in0.size()),
  // static_cast<uint16_t>(sampleRate)); FrequencyDomain fd0 =
  // fft.signalToFrequency(in0, WindowFunction::HANN_WINDOW); FrequencyDomain
  // fd1 = fft.signalToFrequency(in1, WindowFunction::HANN_WINDOW);
  // FrequencyDomain fd2 = fft.signalToFrequency(in2,
  // WindowFunction::HANN_WINDOW);

  // // Stack into STFT-domain object (IMPORTANT: avoid resize() on
  // // vector<FrequencyDomain>)
  // ShortTimeFourierTransformDomain dom(1);
  // dom.stft.reserve(3);
  // dom.stft.emplace_back(fd0);
  // dom.stft.emplace_back(fd1);
  // dom.stft.emplace_back(fd2);

  MelFilter melFilter(numFilters, frameSize, sampleRate);

  std::vector<std::vector<float>> melSpec;
  melFilter.Apply(dom, melSpec);

  DiscreteCosineTransform dct(numCepstral, numFilters);
  std::vector<std::vector<float>> mfccSpec;
  dct.Apply(melSpec, mfccSpec);

  PrincipleComponentAnalysis pca(numPCAComponents, numCepstral);

  std::vector<std::vector<float>> pcaFeatureVector;
  pca.Apply(mfccSpec, pcaFeatureVector);

  return pcaFeatureVector;
}

TEST(LDA, ApplyLDA) {
  // Load MP3 data from file.
  MP3Data data = readMP3File("audio/siren2.mp3");
  const int offset0 = 0;
  const uint16_t numCepstral = 13;
  const uint16_t numPCAComponents = 13;
  const uint16_t numClasses = 4;

  // Apply LDA to the PCA feature vector.

  LinearDiscriminantAnalysis lda(numPCAComponents, numClasses);

  // Create PCA feature vector from MP3 data.
  std::vector<std::vector<float>> pcaFeatureVector =
      Make3FramePCASpecFromMP3(data, SAMPLE_FREQUENCY, offset0);

  // printf("PCA Feature Vector:\n");
  // for (const auto& frame : pcaFeatureVector) {
  //   for (const auto& value : frame) {
  //     printf("%f ", value);
  //   }
  //   printf("\n");
  // }

  std::string predictedClass = lda.Apply(pcaFeatureVector);
  printf("Predicted Class from LDA: %s\n", predictedClass.c_str());
}
