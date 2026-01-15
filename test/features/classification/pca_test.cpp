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

static void Make3FrameMFCCSpecFromMP3(const MP3Data& data, int sampleRate,
                                      int offset0, matrix& mfccSpec,
                                      std::vector<float>& mfccSpecData) {
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

  // Stack into STFT-domain object (IMPORTANT: avoid resize() on
  // vector<FrequencyDomain>)
  ShortTimeFourierTransformDomain dom(3);
  dom.stft.reserve(3);
  dom.stft.emplace_back(fd0);
  dom.stft.emplace_back(fd1);
  dom.stft.emplace_back(fd2);

  MelFilter melFilter(numFilters, frameSize, sampleRate);

  matrix melSpec;
  std::vector<float> melSpecData;
  melFilter.Apply(dom, melSpec, melSpecData);

  DiscreteCosineTransform dct(numCepstral, numFilters);
  dct.Apply(melSpec, mfccSpec, mfccSpecData);
}

TEST(PCA, ApplyPCA) {
  MP3Data data = readMP3File("audio/285_sine.mp3");
  const int offset0 = 100000;
  const uint16_t numCepstral = 13;
  const uint16_t numPCAComponents = 13;

  ASSERT_GT(static_cast<int>(data.channel1.size()),
            offset0 + 3 * static_cast<int>(WAVEFORM_SAMPLES));

  PrincipleComponentAnalysis pca(numPCAComponents, numCepstral);

  matrix mfccSpec;
  std::vector<float> mfccSpecData;
  Make3FrameMFCCSpecFromMP3(data, SAMPLE_FREQUENCY, offset0, mfccSpec,
                            mfccSpecData);

  matrix pcaSpec;
  std::vector<float> pcaSpecData;
  pca.Apply(mfccSpec, pcaSpec, pcaSpecData);

  ASSERT_EQ(pcaSpec.numRows, mfccSpec.numRows);
  ASSERT_EQ(pcaSpec.numCols, numPCAComponents);
}
