/**
 ******************************************************************************
 * @file    sd_writter.h
 * @brief   SD card writer definitions.
 ******************************************************************************
 */

#pragma once

#ifdef STM_BUILD
#include "hardware_interface/sd_logger/FatFs/App/fatfs.h"
#include <string>

/** @brief Class for writing to a SD card on the microcontroller. */
class SDCardWriter {
public:
  /**
   * @brief Construct a new SDCardWriter object.
   *
   * @param filename The name of the file to write data to.
   */
  SDCardWriter(std::string filename);

  /** @brief Destroy the SDCardWriter object. */
  ~SDCardWriter();

  /**
   * @brief Write string to the file.
   *
   * @param text The text to write to the file.
   * @return int Status code. 0 for success, otherwise failure has occurred.
   */
  int write(const char *text);

  int write_int32_buffer(int32_t *buffer, int length);

private:
  /** @brief FAT Filesystem handle. */
  FATFS FatFs;

  /** @brief File handle. */
  FIL fil;

  /** @brief Result after file operations. */
  FRESULT fres;
};

#endif