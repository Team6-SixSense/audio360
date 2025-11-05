/**
 ******************************************************************************
 * @file    sd_writter.cpp
 * @brief   SD card writer source.
 ******************************************************************************
 */
#ifdef STM_BUILD

#include "helper/logging/sd_writer.h"
#include "hardware_interface/system/peripheral.h"
#include "helper/logging/logging.hpp"
#include "string.h"

SDCardWriter::SDCardWriter(std::string filename) {

  // Open the file system.
  this->fres = f_mount(&this->FatFs, "", 1); // 1 = mount now
  if (this->fres != FR_OK) {
    ERROR("f_mount error (%i)\r\n", fres);
    Error_Handler();
  }

  const char *filePath = (filename + ".txt").c_str();

  this->fres = f_open(&this->fil, filePath,
                      FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);

  if (this->fres == FR_OK) {
    INFO("%s opened for writing\r\n", filePath);
  } else {
    ERROR("f_open error (%i)\r\n", this->fres);
  }
}

SDCardWriter::~SDCardWriter() {
  f_close(&this->fil);
  f_mount(NULL, "", 0);
}

int SDCardWriter::write(const char *text) {

  UINT textSize = strlen(text);
  BYTE buffer[256];
  strncpy((char *)buffer, text, textSize);

  UINT bytesWrote;
  this->fres = f_write(&this->fil, buffer, textSize, &bytesWrote);

  if (this->fres == FR_OK) {
    INFO("Wrote %i bytes to SD Card!\r\n", bytesWrote);
  } else {
    ERROR("f_write error (%i)\r\n", bytesWrote);
  }

  f_sync(&this->fil);

  return this->fres;
}

#endif