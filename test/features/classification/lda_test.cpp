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

static void PrintMatrix(const matrix& m) {
  for (uint16_t r = 0; r < m.numRows; ++r) {
    for (uint16_t c = 0; c < m.numCols; ++c) {
      printf("%8.4f ", m.pData[r * m.numCols + c]);
    }
    printf("\n");
  }
}

// Build a 3-frame STFT-domain by computing 3 FFTs from 3 windows of the MP3.
static void MakePCASpecFromMP3(const MP3Data& data, int sampleRate, int offset0,
                               matrix& pcaSpec,
                               std::vector<float>& pcaSpecData) {
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

  ShortTimeFourierTransformDomain dom(numFrames);

  dom.stft.reserve(numFrames);

  for (size_t frame = 0; frame < numFrames; ++frame) {
    const size_t start = static_cast<size_t>(offset0) + frame * frameSize;
    std::vector<float> in(data.channel1.begin() + start,
                          data.channel1.begin() + start + frameSize);
    FrequencyDomain fd = fft.signalToFrequency(in, WindowFunction::HANN_WINDOW);
    dom.stft.emplace_back(fd);
  }

  MelFilter melFilter(numFilters, frameSize, sampleRate);

  matrix melSpec;
  std::vector<float> melSpecData;
  melFilter.Apply(dom, melSpec, melSpecData);

  DiscreteCosineTransform dct(numCepstral, numFilters);
  matrix mfccSpec;
  std::vector<float> mfccSpecData;
  dct.Apply(melSpec, mfccSpec, mfccSpecData);

  PrincipleComponentAnalysis pca(numPCAComponents, numCepstral);

  pca.Apply(mfccSpec, pcaSpec, pcaSpecData);
  printf("PCA Spec Shape: %u x %u\n", pcaSpec.numRows, pcaSpec.numCols);
}

TEST(LDA, ApplyLDA) {
  // Load MP3 data from file.
  MP3Data data = readMP3File("audio/jackhammer.mp3");
  const int offset0 = 0;
  const uint16_t numPCAComponents = 13;
  const uint16_t numClasses = 3;

  // Apply LDA to the PCA feature vector.

  LinearDiscriminantAnalysis lda(numPCAComponents, numClasses);

  // Create PCA feature vector from MP3 data.
  matrix pcaFeatureVector;
  std::vector<float> pcaFeatureVectorData;
  MakePCASpecFromMP3(data, SAMPLE_FREQUENCY, offset0, pcaFeatureVector,
                     pcaFeatureVectorData);
  // PrintMatrix(pcaFeatureVector.numRows, pcaFeatureVector.numCols);

  std::string predictedClass = lda.Apply(pcaFeatureVector);
  printf("Predicted Class from LDA: %s\n", predictedClass.c_str());
}
