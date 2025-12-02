/**
 ******************************************************************************
 * @file    matrix_addition_test.cpp
 * @brief   Unit tests for matrix addition.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "matrix_test.h"

/**
 * @brief Tests matrix commutative property.
 * Property: A + B = B + A.
 */
TEST(MatrixAddition, Commutativity) {
  const int numRows = 4, numCols = 4;
  matrix A, B, AB, BA;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  createRandomMatrix(numRows, numCols, &B);

  matrix_init_f32(&AB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&BA, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Add matrices.
  matrix_add_f32(&A, &B, &AB);
  matrix_add_f32(&B, &A, &BA);

  // Assert that commutativity of matrix addition holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(AB.pData[i], BA.pData[i], PRECISION_ERROR);
  }
}

/**
 * @brief Tests matrix associativitiy property holds.
 * Property: ((A + B) + C = A + (B + C)).
 */
TEST(MatrixAddition, Associativity) {
  const int numRows = 4, numCols = 4;
  matrix A, B, C, AB, BC, ABC1, ABC2;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  createRandomMatrix(numRows, numCols, &B);
  createRandomMatrix(numRows, numCols, &C);

  matrix_init_f32(&AB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&BC, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABC1, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABC2, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Add matrices.
  matrix_add_f32(&A, &B, &AB);  // AB = A + B
  matrix_add_f32(&B, &C, &BC);  // BC = B + C

  matrix_add_f32(&AB, &C, &ABC1);  // (A + B) + C
  matrix_add_f32(&A, &BC, &ABC2);  // A + (B + C)

  // Assert that associativitiy of matrix addition holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(ABC1.pData[i], ABC2.pData[i], PRECISION_ERROR);
  }
}

/**
 *  @brief Tests matrix additive identity property holds.
 * Property: (A + 0 = A).
 */
TEST(MatrixAddition, AdditiveIdentity) {
  const int numRows = 5, numCols = 5;
  matrix A, Zero, Result;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);

  float32_t* zeros = (float32_t*)calloc(numRows * numCols, sizeof(float32_t));
  matrix_init_f32(&Zero, numRows, numCols, zeros);

  matrix_init_f32(&Result, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Add matrices.
  matrix_add_f32(&A, &Zero, &Result);

  // Assert that matrix additive identity property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(Result.pData[i], A.pData[i], PRECISION_ERROR);
  }
}

/**
 *  @brief Tests matrix additive inverse property holds.
 * Property: (A + (−A) = 0).
 */
TEST(MatrixAddition, AdditiveInverse) {
  const int numRows = 4, numCols = 4;
  matrix A, NegA, Result;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);

  float32_t* negData = (float32_t*)calloc(numRows * numCols, sizeof(float32_t));
  for (int i = 0; i < numRows * numCols; i++) {
    negData[i] = -A.pData[i];
  }
  matrix_init_f32(&NegA, numRows, numCols, negData);

  matrix_init_f32(&Result, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Add matrices.
  matrix_add_f32(&A, &NegA, &Result);

  // Assert that the matric inverse property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(Result.pData[i], 0.0f, PRECISION_ERROR);
  }
}

/** @brief Test that matrix addtiion failes when matrices size are not
 * compatible. */
TEST(MatrixAddition, DimensionMismatch) {
  matrix A, B, C;

  // Create matrices.
  createRandomMatrix(3, 3, &A);
  createRandomMatrix(4, 4, &B);
  matrix_init_f32(&C, 3, 3, (float32_t*)calloc(9, sizeof(float32_t)));

  // Expect matrix addition failure due to incompatible matrix sizes.
  arm_status status = matrix_add_f32(&A, &B, &C);
  ASSERT_EQ(status, ARM_MATH_SIZE_MISMATCH);
}
