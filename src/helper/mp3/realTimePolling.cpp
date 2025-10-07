#include "helper/mp3/realTimePolling.h"

RealTimePolling::RealTimePolling(size_t windowSize, std::vector<double> data)
    : windowSize(windowSize), data(data) {

  this->dataSize = static_cast<int>(this->data.size());
}

bool RealTimePolling::isDone() {

  return ((this->currentPos + 1) >= this->dataSize);
}

std::vector<double> RealTimePolling::getCurrentData() {

  std::vector<double> currentData(this->data.begin() + this->currentPos -
                                      this->windowSize,
                                  this->data.begin() + this->currentPos + 1);
  this->currentPos++;

  return currentData;
}