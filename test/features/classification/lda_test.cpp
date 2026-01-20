/**
 ******************************************************************************
 * @file    lda_test.cpp
 * @brief   Unit tests for LinearDiscriminantAnalysis classification.
 ******************************************************************************
 */

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
#include "matrix.h"
#include "mel_filter.h"
#include "mp3.h"
#include "pca.h"

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

/** @brief Builds a power STFT matrix from FFT frames. */
static void GenerateSTFT(const std::vector<FrequencyDomain>& audioSignal,
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

/** @brief Builds a PCA feature matrix from an MP3 segment. */
static void MakePCASpecFromMP3(const MP3Data& data, int sampleRate, int offset0,
                               matrix& pcaSpec,
                               std::vector<float>& pcaFeatureVector) {
  const uint16_t frameSize = WAVEFORM_SAMPLES;
  const uint16_t numFilters = 40;
  const uint16_t numCepstral = 13;
  const uint16_t numPCAComponents = 13;

  if (offset0 < 0 || static_cast<size_t>(offset0) >= data.channel1.size()) {
    return;
  }
  const size_t availableSamples =
      data.channel1.size() - static_cast<size_t>(offset0);
  const uint16_t numFrames = availableSamples / frameSize;
  if (numFrames == 0) {
    return;
  }

  FFT fft(static_cast<uint16_t>(frameSize), static_cast<uint16_t>(sampleRate));

  std::vector<FrequencyDomain> dom;

  for (size_t frame = 0; frame < numFrames; ++frame) {
    const size_t start = static_cast<size_t>(offset0) + frame * frameSize;
    std::vector<float> in(data.channel1.begin() + start,
                          data.channel1.begin() + start + frameSize);
    FrequencyDomain fd = fft.signalToFrequency(in, WindowFunction::HANN_WINDOW);
    dom.push_back(fd);
  }

  matrix stftMatrix;
  std::vector<float> stftDataVector;
  GenerateSTFT(dom, stftMatrix, stftDataVector);

  MelFilter melFilter(numFilters, frameSize, sampleRate);

  matrix melSpec;
  std::vector<float> melSpectrogramVector;
  melFilter.apply(stftMatrix, melSpec, melSpectrogramVector);

  DiscreteCosineTransform dct(numCepstral, numFilters);
  matrix mfccSpec;
  std::vector<float> mfccSpectrogramVector;
  dct.apply(melSpec, mfccSpec, mfccSpectrogramVector);

  PrincipleComponentAnalysis pca(numPCAComponents, numCepstral);

  pca.apply(mfccSpec, pcaSpec, pcaFeatureVector);
}

/** @brief Runs LDA classification on PCA features from an MP3 file. */
TEST(LDA, ApplyLDA) {
  // Load MP3 data from file.
  MP3Data data = readMP3File("audio/jackhammer.mp3");
  const int offset0 = 0;
  const uint16_t numPCAComponents = 13;
  const uint16_t numClasses = 3;

  // Apply LDA to the PCA feature vector.

  LinearDiscriminantAnalysis lda(numPCAComponents, numClasses);

  // Create PCA feature vector from MP3 data.
  matrix pcaFeature;
  std::vector<float> pcaFeatureVector;
  MakePCASpecFromMP3(data, SAMPLE_FREQUENCY, offset0, pcaFeature,
                     pcaFeatureVector);

  std::string predictedClass = lda.apply(pcaFeature);
}
