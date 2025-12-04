/**
 ******************************************************************************
 * @file    matrix_creation_test.cpp
 * @brief   Unit tests for matrix creation/initialization.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "matrix_test.h"
#include "test_helper.h"

/** @brief Test for creating an identity matrix */
TEST(MatrixCreation, Indentity) {
  const int MIN_NUM = 5;
  const int MAX_NUM = 100;

  const int N = generateRandomInt(MIN_NUM, MAX_NUM);
  matrix M;

  // Create indentity matrix.
  createIdentityMatrix(N, &M);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      // Assert initialiation of identity matrix by checking diagonal components
      // are 1 and the rest are 0.
      if (i != j) {
        ASSERT_NEAR(M.pData[i * N + j], 0.0, PRECISION_ERROR);
      } else {
        ASSERT_NEAR(M.pData[i * N + j], 1.0, PRECISION_ERROR);
      }
    }
  }
}
