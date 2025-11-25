/**
 ******************************************************************************
 * @file    realTimePolling_test.cpp
 * @brief   Unit tests for real time polling for pre-recorded audio data.
 ******************************************************************************
 */

#include "helper/mp3/realTimePolling.h"

#include <gtest/gtest.h>

#include <limits>

/** @brief Given a data of pre-recorded data and window size 2. The real time
 * polling can simulate real time data without any memory errors.*/
TEST(RealTimePolling, RealTimePollingMemory) {
  // Prepare pre-recorded data and real time processing object.
  std::vector<double> preRecordedData = {1.0, 2.0, 3.0, 4.0, 5.0,
                                         6.0, 7.0, 8.0, 9.0, 10.0};
  RealTimePolling realTimePolling = RealTimePolling(2, preRecordedData);

  // Expect to simulate real time data N - 1 times where N is the size of
  // preRecordedData.
  double num = 1.0;
  for (int i = 0; i < (preRecordedData.size() - 1); i++) {
    ASSERT_FALSE(realTimePolling.isDone());

    std::vector<double> realTimeData = realTimePolling.getCurrentData();
    std::vector<double> expectedRealTimeData = {num, num + 1.0};

    ASSERT_EQ(2, realTimeData.size());

    EXPECT_NEAR(expectedRealTimeData[0], realTimeData[0],
                std::numeric_limits<float>::epsilon());
    EXPECT_NEAR(expectedRealTimeData[1], realTimeData[1],
                std::numeric_limits<float>::epsilon());

    num += 1.0;
  }

  // Expect the real time data to be completed.
  ASSERT_TRUE(realTimePolling.isDone());
}
