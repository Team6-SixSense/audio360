/**
 ******************************************************************************
 * @file    test_helper.cpp
 * @brief   Helper functions source file for testing.
 ******************************************************************************
 */

#include "test_helper.h"

const float PRECISION_ERROR = 0.000001f;

int generateRandomInt(int minNum, int maxNum) {
  float norm = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

  return static_cast<int>(minNum + norm * (maxNum - minNum + 1));
}

float generateRandomFloat32(float minNum, float maxNum) {
  float norm = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  return maxNum + norm * (maxNum - minNum);
}
