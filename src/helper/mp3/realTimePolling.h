/**
 ******************************************************************************
 * @file    realTimePolling.h
 * @brief   Real-time polling header.
 ******************************************************************************
 */

#pragma once

#include <vector>

/**
 * @brief Simulates real-time data polling from a pre-recorded dataset.
 *
 * This class provides an interface to emulate streaming data by iterating
 * through a vector of known values, returning a moving window of samples at
 * each simulated time step.
 */
class RealTimePolling {
 public:
  /**
   * @brief Constructs a new RealTimePolling object.
   *
   * @param windowSize The number of samples to include in each simulated
   * real-time window.
   * @param data The complete vector of known (pre-recorded) data values.
   */
  RealTimePolling(size_t windowSize, std::vector<double> data);

  /**
   * @brief Checks whether all available data has been processed.
   *
   * @return true  If all data points have been used.
   * @return false If more data remains for simulation.
   */
  bool isDone();

  /**
   * @brief Retrieves the current simulated real-time data window.
   *
   * Returns a vector containing the most recent @ref windowSize samples,
   * including the current time step and the preceding (windowSize - 1) samples,
   * ordered chronologically.
   *
   * @return std::vector<double> The simulated real-time data window.
   */
  std::vector<double> getCurrentData();

 private:
  /** @brief The number of samples in each simulated real-time window. */
  size_t windowSize{0};

  /** @brief The complete set of pre-recorded data used for simulation. */
  std::vector<double> data{};

  /** @brief The total number of samples in @ref data. */
  int dataSize{0};

  /**
   * @brief The current index within @ref data representing the present time
   * step. This index determines the newest sample in the current real-time
   * window.
   */
  int currentPos{0};
};