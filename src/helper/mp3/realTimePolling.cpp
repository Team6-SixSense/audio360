/**
 ******************************************************************************
 * @file    realTimePolling.cpp
 * @brief   Real-time polling source code.
 ******************************************************************************
 */

#include "helper/mp3/realTimePolling.h"

RealTimePolling::RealTimePolling(size_t windowSize, std::vector<double> data)
    : windowSize(windowSize), data(data) {
  this->dataSize = static_cast<int>(this->data.size());

  if (windowSize == 0 || windowSize > this->dataSize) {
    this->windowSize = this->dataSize;
  }

  this->currentPos = this->windowSize;
}

bool RealTimePolling::isDone() { return (this->currentPos > this->dataSize); }

std::vector<double> RealTimePolling::getCurrentData() {
  if (isDone()) {
    return {};
  }

  std::vector<double> currentData(
      this->data.begin() + this->currentPos - this->windowSize,
      this->data.begin() + this->currentPos);
  this->currentPos++;

  return currentData;
}