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
#include "matrix.h"
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

static void PrintMatrix(const matrix& m) {
  for (uint16_t r = 0; r < m.numRows; ++r) {
    for (uint16_t c = 0; c < m.numCols; ++c) {
      printf("%8.4f ", m.pData[r * m.numCols + c]);
    }
    printf("\n");
  }
}

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

static void PrintSTFTMagnitude(const std::vector<FrequencyDomain>& dom) {
  if (dom.empty()) {
    printf("(empty)\n");
    return;
  }

  const size_t numFrames = dom.size();
  const size_t numBins = dom[0].magnitude.size();

  for (size_t frame = 0; frame < numFrames; ++frame) {
    // Optional: sanity check bin size per frame
    // if (dom[frame].magnitude.size() != numBins) continue;

    for (size_t bin = 0; bin < numBins; ++bin) {
      printf("%8.4f ", dom[frame].magnitude[bin]);
    }
    printf("\n");
  }
}

// Build a 3-frame STFT-domain by computing 3 FFTs from 3 windows of the MP3.
static void Make3FrameMelSpecFromMP3(const MP3Data& data, int sampleRate,
                                     int offset0, matrix& melSpec,
                                     std::vector<float>& melSpectrogramVector) {
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

  std::vector<FrequencyDomain> dom;
  dom.push_back(fd0);
  dom.push_back(fd1);
  dom.push_back(fd2);

  matrix stftMatrix;
  std::vector<float> stftDataVector;
  GenerateSTFT(dom, stftMatrix, stftDataVector);

  MelFilter melFilter(numFilters, frameSize, sampleRate);

  melFilter.Apply(stftMatrix, melSpec, melSpectrogramVector);
}

TEST(DCTTest, ComputeDCT_OnMelSpectrogram_ProducesExpectedMFCC) {
  // These were validated experimentally using Python to see if relatively the
  // MFCC are obtained.
  std::vector<float> actualFrame1 = {
      -56.6641, 20.2349, 20.8206, 15.1366, 10.8139, 7.1945, 4.3752,
      3.0178,   1.4158,  0.7228,  -0.0174, -1.6363, -2.0123};
  std::vector<float> actualFrame2 = {
      -56.8101, 20.1327, 20.8456, 15.5742, 11.3821, 7.3043, 4.4060,
      2.4799,   0.9329,  0.1191,  0.3618,  -1.2808, -2.3173};
  std::vector<float> actualFrame3 = {
      -56.2733, 20.8818, 21.4201, 15.7347, 10.5507, 6.1766, 3.1375,
      2.4075,   1.4275,  0.8910,  0.6098,  -0.9236, -2.0151};

  MP3Data data = readMP3File("audio/285_sine.mp3");
  const int offset0 = 100000;  // same style as fft_test.cpp

  const uint16_t numCepstral = 13;
  const uint16_t numFilters = 40;

  DiscreteCosineTransform dct(numCepstral, numFilters);

  matrix melSpec;
  std::vector<float> melSpectrogramVector;
  Make3FrameMelSpecFromMP3(data, SAMPLE_FREQUENCY, offset0, melSpec,
                           melSpectrogramVector);

  printf("MelSpec\n");
  PrintMatrix(melSpec);

  matrix mfccSpec;
  std::vector<float> mfccSpectrogramVector;
  dct.Apply(melSpec, mfccSpec, mfccSpectrogramVector);

  printf("MFCCSpec\n");
  PrintMatrix(mfccSpec);

  ASSERT_EQ(mfccSpec.numCols, numCepstral);

  for (int i = 0; i < numCepstral; ++i) {
    EXPECT_NEAR(mfccSpec.pData[i], actualFrame1[i], 1e-3f)
        << "Mismatch at frame 0, coeff " << i;
    EXPECT_NEAR(mfccSpec.pData[numCepstral + i], actualFrame2[i], 1e-3f)
        << "Mismatch at frame 1, coeff " << i;
    EXPECT_NEAR(mfccSpec.pData[2 * numCepstral + i], actualFrame3[i], 1e-3f)
        << "Mismatch at frame 2, coeff " << i;
  }
}
