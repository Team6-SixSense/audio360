#include "helper/mp3/minimp3.h"
#include "helper/mp3/minimp3_ex.h"
#include <iostream>
#include <vector>

/** @brief Classification of MP3 channels. */
enum class Channel {
  Mono = 1,
  Stereo = 2,
};

struct MP3Data {

  /** @brief The number of sample of MP3 data. */
  size_t numSamples;

  /** @brief Channel classification. */
  Channel channel;

  /** @brief Holdd PCM data of MP3. Holds left channel if stereo. */
  std::vector<double> channel1;

  /** @brief Holdd PCM data of MP3. Holds right channel if stereo. */
  std::vector<double> channel2;
};

/**
 * @brief Reads and processes MP3 file.
 *
 * @param filename path to MP3 file. This is relative to where the binary is
 * being ran.
 * @param shouldPlot True to plot audio wave. False otherwise.
 * @return MP3Data PCM of MP3 file data.
 */
MP3Data readMP3File(std::string filepath, bool shouldPlot = false);

/**
 * @brief Read MP3 file and return raw binary contents.
 *
 * @param filepath path to MP3 file. This is relative to where the binary is
 * being ran.
 * @return std::vector<unsigned char> byte MP3 data from file.
 */
std::vector<unsigned char> readRawMP3(std::string filepath);

/**
 * @brief Handles mono channel MP3 processing.
 *
 * @param info MP3 file info indentifies by minimp3 library.
 * @param shouldPlot True to plot audio wave. False otherwise.
 * @return MP3Data PCM of MP3 file data.
 */
MP3Data handleMonoChannel(mp3dec_file_info_t &info, bool shouldPlot);

/**
 * @brief Handles stereo channel MP3 processing.
 *
 * @param info MP3 file info indentifies by minimp3 library.
 * @param shouldPlot True to plot audio wave. False otherwise.
 * @return MP3Data PCM of MP3 file data.
 */
MP3Data handleStereoChannel(mp3dec_file_info_t &info, bool shouldPlot);