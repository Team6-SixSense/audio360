/**
 ******************************************************************************
 * @file    test_helper.h
 * @brief   Helper functions header file for testing.
 ******************************************************************************
 */

#pragma once

#include <cstdlib>

extern const float PRECISION_ERROR;

/**
 * @brief Generate a random integer.
 *
 * @param minNum Minimum number.
 * @param maxNum Maximum number
 * @return int randomly generated number between @ref minNum and @ref maxNum
 * inclusive.
 */
int generateRandomInt(int minNum = 0, int maxNum = RAND_MAX);

/**
 * @brief Generate a random float32.
 *
 * @param minNum Minimum number.
 * @param maxNum Maximum number
 * @return float randomly generated number between @ref minNum and @ref
 * maxNum inclusive.
 */
float generateRandomFloat32(float minNum = 0.0,
                            float maxNum = static_cast<float>(RAND_MAX));
