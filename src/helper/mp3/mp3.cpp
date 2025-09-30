
#define MINIMP3_IMPLEMENTATION

#include "helper/mp3/mp3.h"
// #include "helper/plot/plot.hpp"
#include <algorithm>
#include <fstream>
#include <stdio.h>

MP3Data readMP3File(std::string filepath) {

  std::vector<unsigned char> rawMp3Data = readRawMP3(filepath);

  // Decode MP3 bnary to Pulse Code Modulation (PCM).
  // PCM allows to represent analog signals digitally.
  mp3dec_t dec;
  mp3dec_file_info_t info; // PCM will be stored here

  mp3dec_init(&dec);
  int statusCode = mp3dec_load_buf(&dec, rawMp3Data.data(), rawMp3Data.size(),
                                   &info, nullptr, nullptr);

  if (statusCode != 0) {
     printf("[ERROR] Error in decoding MP3 binary data.");
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

  return data;
}

std::vector<unsigned char> readRawMP3(std::string filepath) {
  // Open the file in binary mode.
  std::ifstream file(filepath, std::ios::binary);

  if (!file) {
    printf("[Error] Could not find file %s", filepath.c_str());
  } else if (!file.is_open()) {
    printf("[Error] Could not open file %s",filepath.c_str());
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

MP3Data handleMonoChannel(mp3dec_file_info_t &info) {
  printf("[INFO] Mono Channel");

  std::vector<int16_t> pcm(info.buffer, info.buffer + info.samples);

  std::vector<double> normalizedPcm(info.samples);
  std::vector<double> xAxis(info.samples);
  double timestep = 1.0 / info.hz;
  double timeStamp = 0.0;

  // Normalize and store MP3 data into a vector.
  for (size_t i = 0; i < info.samples; i++) {
    normalizedPcm[i] = static_cast<double>(pcm[i]) / INT16_MAX;
    timeStamp += timestep;
    xAxis[i] = timeStamp;
  }

  MP3Data data = {info.samples, Channel::Mono, normalizedPcm, {}};
  return data;
}

MP3Data handleStereoChannel(mp3dec_file_info_t &info) {
  printf("[INFO] Stereo Channel");

  std::vector<int16_t> pcm(info.buffer, info.buffer + info.samples);

  size_t numSamples = info.samples / 2;
  std::vector<double> normalizedLeftPcm(numSamples);
  std::vector<double> normalizedRightPcm(numSamples);
  std::vector<double> xAxis(numSamples);
  double timestep = 1.0 / info.hz;
  double timeStamp = 0.0;

  // Processing intertwining channel sample, normalizing, and store MP3 data
  // into vector.
  for (size_t i = 0; i < numSamples; i++) {
    normalizedLeftPcm[i] = static_cast<double>(pcm[2 * i]) / INT16_MAX;
    normalizedRightPcm[i] = static_cast<double>(pcm[2 * i + 1]) / INT16_MAX;
    timeStamp += timestep;
    xAxis[i] = timeStamp;
  }

  MP3Data data = {info.samples, Channel::Stereo, normalizedLeftPcm,
                  normalizedRightPcm};
  return data;
}