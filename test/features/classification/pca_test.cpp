/**
 ******************************************************************************
 * @file    pca_test.cpp
 * @brief   Unit tests for PrincipleComponentAnalysis feature projection.
 ******************************************************************************
 */

#include "pca.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>
#include <window.hpp>

#include "constants.h"
#include "dct.h"
#include "fft.h"
#include "matrix.h"
#include "mel_filter.h"
#include "mp3.h"

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

/** @brief Builds a 3-frame MFCC spectrogram from an MP3 segment. */
static void Make3FrameMFCCSpecFromMP3(
    const MP3Data& data, int sampleRate, int offset0, matrix& mfccSpec,
    std::vector<float>& mfccSpectrogramVector) {
  const uint16_t frameSize = WAVEFORM_SAMPLES;
  const uint16_t numFilters = 40;
  const uint16_t numCepstral = 13;

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

  matrix stftMatrix;
  std::vector<float> stftDataVector;
  GenerateSTFT(dom, stftMatrix, stftDataVector);

  MelFilter melFilter(numFilters, frameSize, sampleRate);

  matrix melSpec;
  std::vector<float> melSpectrogramVector;
  melFilter.apply(stftMatrix, melSpec, melSpectrogramVector);

  DiscreteCosineTransform dct(numCepstral, numFilters);
  dct.apply(melSpec, mfccSpec, mfccSpectrogramVector);
}

/** @brief Verifies PCA output dimensions match the requested components. */
TEST(PCA, ApplyPCA) {
  MP3Data data = readMP3File("audio/285_sine.mp3");
  const int offset0 = 100000;
  const uint16_t numCepstral = 13;
  const uint16_t numPCAComponents = 13;

  ASSERT_GT(static_cast<int>(data.channel1.size()),
            offset0 + 3 * static_cast<int>(WAVEFORM_SAMPLES));

  PrincipleComponentAnalysis pca(numPCAComponents, numCepstral);

  matrix mfccSpec;
  std::vector<float> mfccSpectrogramVector;
  Make3FrameMFCCSpecFromMP3(data, SAMPLE_FREQUENCY, offset0, mfccSpec,
                            mfccSpectrogramVector);

  matrix pcaSpec;
  std::vector<float> pcaFeatureVector;
  pca.apply(mfccSpec, pcaSpec, pcaFeatureVector);

  ASSERT_EQ(pcaSpec.numRows, mfccSpec.numRows);
  ASSERT_EQ(pcaSpec.numCols, numPCAComponents);
}
