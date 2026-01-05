/**
 ******************************************************************************
 * @file    matrix_subtraction_test.cpp
 * @brief   Unit tests for matrix subtraction.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "matrix_test.h"

/**
 *  @brief Tests matrix anti-commutative  property holds.
 * Property: A – B = –(B – A).
 */
TEST(MatrixSubtraction, AntiCommutative) {
  const int numRows = 4, numCols = 4;
  matrix A, B, R1, R2;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  createRandomMatrix(numRows, numCols, &B);

  matrix_init_f32(&R1, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  matrix_init_f32(&R2, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Subtract matrices.
  matrix_sub_f32(&A, &B, &R1);
  matrix_sub_f32(&B, &A, &R2);

  // Assert anti-commutative property holds
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(R1.pData[i], -R2.pData[i], PRECISION_ERROR);
  }
}

/**
 *  @brief Tests matrix subtractive identity property holds.
 * Property: A - 0 = A.
 */
TEST(MatrixSubtraction, Identity) {
  const int numRows = 4, numCols = 4;
  matrix A, Zero, Result;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);

  float32_t* zeros = (float32_t*)calloc(numRows * numCols, sizeof(float32_t));
  matrix_init_f32(&Zero, numRows, numCols, zeros);

  matrix_init_f32(&Result, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Subtract matrices.
  matrix_sub_f32(&A, &Zero, &Result);

  // Assert that matrix subtractive identity property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(Result.pData[i], A.pData[i], PRECISION_ERROR);
  }
}

/**
 *  @brief Tests matrix subtractive inverse property holds in subtraction
 * operation.
 * Property: A − A = 0.
 */
TEST(MatrixSubtraction, Inverse) {
  const int numRows = 4, numCols = 4;
  matrix A, Zero;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  matrix_init_f32(&Zero, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Subtract matrices.
  matrix_sub_f32(&A, &A, &Zero);

  // Assert that the matrix inverse property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(Zero.pData[i], 0.0f, PRECISION_ERROR);
  }
}

/** @brief Test that matrix subtraction fails when matrices size are not
 * compatible. */
TEST(MatrixSubtraction, DimensionMismatch) {
  matrix A, B, C;

  // Create matrices.
  createRandomMatrix(4, 4, &A);
  createRandomMatrix(3, 3, &B);
  matrix_init_f32(&C, 4, 4, (float32_t*)calloc(16, sizeof(float32_t)));

  // Expect matrix subtraction failure due to incompatible matrix sizes.
  arm_status status = matrix_sub_f32(&A, &B, &C);
  ASSERT_EQ(status, ARM_MATH_SIZE_MISMATCH);
}