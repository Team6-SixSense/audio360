/**
 ******************************************************************************
 * @file    mel_test.cpp
 * @brief   Unit tests for MelFilter creation and application.
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
#include "matrix.h"
#include "mel_filter.h"
#include "mp3.h"

/** @brief Returns the index of the largest value (0 if empty). */
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

/** @brief Returns the maximum value in the vector (0 if empty). */
static float MaxVal(const std::vector<float>& v) {
  return v.empty() ? 0.0f : *std::max_element(v.begin(), v.end());
}

/** @brief Builds a power STFT matrix from FFT frames. */
void GenerateSTFT(const std::vector<FrequencyDomain>& audioSignal,
                  matrix& stftData, std::vector<float>& stftDataVector) {
  const uint16_t numFrames = static_cast<uint16_t>(audioSignal.size());
  const uint16_t numFreqBins = WAVEFORM_SAMPLES / 2 + 1;

  stftDataVector.assign(numFrames * numFreqBins, 0.0f);
  matrix_init_f32(&stftData, numFrames, numFreqBins, stftDataVector.data());

  for (uint16_t frame = 0; frame < numFrames; ++frame) {
    const size_t rowStart = static_cast<size_t>(frame) * numFreqBins;
    for (uint16_t bin = 0; bin < numFreqBins; ++bin) {
      stftData.pData[rowStart + bin] =
          std::pow(audioSignal[frame].magnitude[bin], 2.0f);
    }
  }
}

/** @brief Builds a 3-frame STFT matrix from an MP3 channel segment. */
static void Make3FrameSTFTFromMP3(const MP3Data& data, int sampleRate,
                                  int offset0, matrix& stftMatrix,
                                  std::vector<float>& stftDataVector) {
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

  std::vector<FrequencyDomain> dom;
  dom.push_back(fd0);
  dom.push_back(fd1);
  dom.push_back(fd2);

  GenerateSTFT(dom, stftMatrix, stftDataVector);
}

/** @brief Counts values greater than eps in a vector. */
static int CountPositive(const std::vector<float>& v, float eps = 1e-8f) {
  int c = 0;
  for (float x : v)
    if (x > eps) ++c;
  return c;
}

/** @brief Counts values greater than eps in a matrix row. */
static int CountPositiveRow(const matrix& m, uint16_t row, float eps = 1e-8f) {
  int c = 0;
  const uint16_t cols = m.numCols;
  const size_t rowStart = static_cast<size_t>(row) * cols;
  for (uint16_t col = 0; col < cols; ++col) {
    if (m.pData[rowStart + col] > eps) {
      ++c;
    }
  }
  return c;
}

/** @brief Verifies mel filter responses are sparse per impulse bin. */
TEST(MelFilterTest, CreateFilterBank_ImpulseBinContributesToAtMostTwoMelBands) {
  MP3Data data = readMP3File("audio/285_sine.mp3");

  const int offset0 = 100000;  // same style as fft_test.cpp
  ASSERT_GT(static_cast<int>(data.channel1.size()),
            offset0 + 3 * static_cast<int>(WAVEFORM_SAMPLES));

  matrix stft;
  std::vector<float> stftDataVector;
  Make3FrameSTFTFromMP3(data, SAMPLE_FREQUENCY, offset0, stft, stftDataVector);

  const int stftPositives = CountPositiveRow(stft, 1, 0.5f);
  ;

  const uint16_t fftSize = WAVEFORM_SAMPLES;  // matches FFT output size
  const uint16_t numFilters = 40;

  MelFilter mel(numFilters, fftSize, SAMPLE_FREQUENCY);

  matrix melSpec;
  std::vector<float> melSpectrogramVector;
  mel.apply(stft, melSpec, melSpectrogramVector);

  ASSERT_EQ(melSpec.numRows, stft.numRows);
  ASSERT_EQ(melSpec.numCols, numFilters);

  // We should expect the mel bands to be more sparse compared to STFT bins.
  const int melPositives = CountPositiveRow(melSpec, 1, 0.5f);
  EXPECT_LE(melPositives, stftPositives);
}
