/**
 ******************************************************************************
 * @file    mp3.cpp
 * @brief   MP3 processing functions.
 ******************************************************************************
 */

#define MINIMP3_IMPLEMENTATION

#include "mp3.h"

#include <stdio.h>

#include <algorithm>
#include <cmath>
#include <fstream>

#include "constants.h"

namespace {

// Simple linear resampler from srcHz to SAMPLE_FREQUENCY.
std::vector<double> resampleToTarget(const std::vector<double>& in, int srcHz) {
  if (srcHz == SAMPLE_FREQUENCY || in.empty()) {
    return in;
  }

  const double ratio =
      static_cast<double>(SAMPLE_FREQUENCY) / static_cast<double>(srcHz);
  const size_t outLen =
      static_cast<size_t>(std::ceil(static_cast<double>(in.size()) * ratio));

  std::vector<double> out(outLen, 0.0);
  for (size_t i = 0; i < outLen; ++i) {
    const double srcPos = static_cast<double>(i) / ratio;
    const size_t idx = static_cast<size_t>(srcPos);
    if (idx + 1 >= in.size()) {
      out[i] = in.back();
    } else {
      const double frac = srcPos - static_cast<double>(idx);
      out[i] = in[idx] * (1.0 - frac) + in[idx + 1] * frac;
    }
  }
  return out;
}

}  // namespace

MP3Data readMP3File(std::string filepath, bool resampleTo16k) {
  std::vector<unsigned char> rawMp3Data = readRawMP3(filepath);

  // Decode MP3 bnary to Pulse Code Modulation (PCM).
  // PCM allows to represent analog signals digitally.
  mp3dec_t dec;
  mp3dec_file_info_t info;  // PCM will be stored here

  mp3dec_init(&dec);
  int statusCode = mp3dec_load_buf(&dec, rawMp3Data.data(), rawMp3Data.size(),
                                   &info, nullptr, nullptr);

  if (statusCode != 0) {
    printf("[ERROR] Error in decoding MP3 binary data.\n");
  }

  printf("Decoded %d samples\n", info.samples);
  printf("Sample rate: %d Hz\n", info.hz);
  printf("Channels: %d\n", info.channels);

  // Process data based on channel type.
  Channel channel = static_cast<Channel>(info.channels);
  MP3Data data;

  if (channel == Channel::Mono) {
    data = handleMonoChannel(info);

  } else if (channel == Channel::Stereo) {
    data = handleStereoChannel(info);
  }

  // Optionally resample to match the classification pipeline sample rate.
  if (resampleTo16k && info.hz != SAMPLE_FREQUENCY) {
    if (channel == Channel::Mono) {
      data.channel1 = resampleToTarget(data.channel1, info.hz);
      data.numSamples = data.channel1.size();
    } else if (channel == Channel::Stereo) {
      data.channel1 = resampleToTarget(data.channel1, info.hz);
      data.channel2 = resampleToTarget(data.channel2, info.hz);
      data.numSamples = data.channel1.size() * 2;
    }
  }

  return data;
}

std::vector<unsigned char> readRawMP3(std::string filepath) {
  // Open the file in binary mode.
  std::ifstream file(filepath, std::ios::binary);

  if (!file) {
    printf("[Error] Could not find file %s\n", filepath.c_str());
  } else if (!file.is_open()) {
    printf("[Error] Could not open file %s\n", filepath.c_str());
  }

  // Determine the file size.
  file.seekg(0, file.end);
  int fileSize = static_cast<int>(file.tellg());
  file.seekg(0, file.beg);

  // Store raw binary to a vector.
  std::vector<char> signedRawMp3Data(fileSize);
  std::vector<unsigned char> rawMp3Data(fileSize);

  file.read(signedRawMp3Data.data(), fileSize);
  file.close();

  std::transform(signedRawMp3Data.begin(), signedRawMp3Data.end(),
                 rawMp3Data.begin(),
                 [](char c) { return static_cast<unsigned char>(c); });

  return rawMp3Data;
}

MP3Data handleMonoChannel(mp3dec_file_info_t& info) {
  printf("[INFO] Mono Channel\n");

  std::vector<int16_t> pcm(info.buffer, info.buffer + info.samples);

  std::vector<double> normalizedPcm(info.samples);

  // Normalize and store MP3 data into a vector.
  for (size_t i = 0; i < info.samples; i++) {
    normalizedPcm[i] = static_cast<double>(pcm[i]) / INT16_MAX;
  }

  MP3Data data = {info.samples, Channel::Mono, normalizedPcm, {}};
  return data;
}

MP3Data handleStereoChannel(mp3dec_file_info_t& info) {
  printf("[INFO] Stereo Channel\n");

  std::vector<int16_t> pcm(info.buffer, info.buffer + info.samples);

  size_t numSamples = info.samples / 2;
  std::vector<double> normalizedLeftPcm(numSamples);
  std::vector<double> normalizedRightPcm(numSamples);

  // Processing intertwining channel sample, normalizing, and store MP3 data
  // into vector.
  for (size_t i = 0; i < numSamples; i++) {
    normalizedLeftPcm[i] = static_cast<double>(pcm[2 * i]) / INT16_MAX;
    normalizedRightPcm[i] = static_cast<double>(pcm[2 * i + 1]) / INT16_MAX;
  }

  MP3Data data = {info.samples, Channel::Stereo, normalizedLeftPcm,
                  normalizedRightPcm};
  return data;
}
