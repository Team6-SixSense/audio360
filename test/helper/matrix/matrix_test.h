/**
 ******************************************************************************
 * @file    matrix_test.h
 * @brief   Helper functions header code for matrix tests.
 ******************************************************************************
 */

#pragma once

#include <cstdlib>
#include <limits>

#include "matrix.h"
#include "test_helper.h"

/**
 * @brief Create a Identity Matrix object
 *
 * @param n The number of rows. This is equal to the number of columns.
 * @param M matrix instance.
 */
void createIdentityMatrix(int n, matrix* M);

/**
 * @brief Create a Random Matrix of size (numRows, numCols).
 *
 * @param numRows Number of rows in matrix.
 * @param numCols Number of columns in matrix.
 * @param M matrix instance
 * @param minVal The minimum value in the matrix.
 * @param maxVal The maximum value in the matrix.
 */
void createRandomMatrix(int numRows, int numCols, matrix* M,
                        float32_t minVal = -10.0f, float32_t maxVal = 10.0f);
