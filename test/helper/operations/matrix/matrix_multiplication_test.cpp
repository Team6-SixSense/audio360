/**
 ******************************************************************************
 * @file    matrix_multiplication_test.cpp
 * @brief   Unit tests for matrix multiplication.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "matrix_test.h"

/**
 * @brief Tests matrix multiplication associative property.
 * Property: (AB)C = A(BC).
 */
TEST(MatrixMultiplication, Associative) {
  const int numRows = 4, numCols = 4;
  matrix A, B, C, AB, BC, ABC1, ABC2;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A, -5, 5);
  createRandomMatrix(numRows, numCols, &B, -5, 5);
  createRandomMatrix(numRows, numCols, &C, -5, 5);

  matrix_init_f32(&AB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&BC, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABC1, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABC2, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Multiply matrices.
  matrix_mult_f32(&A, &B, &AB);
  matrix_mult_f32(&AB, &C, &ABC1);

  matrix_mult_f32(&B, &C, &BC);
  matrix_mult_f32(&A, &BC, &ABC2);

  // Assert that associative of matrix multiplication holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(ABC1.pData[i], ABC2.pData[i], MATRIX_MULT_PRECISION);
  }
}

/**
 * @brief Tests matrix multiplication distributive property.
 * Property: A(B + C) = AB + AC.
 */
TEST(MatrixMultiplication, Distributive) {
  const int numRows = 4, numCols = 4;
  matrix A, B, C, BC, AB, AC, ABC1, ABC2;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A, -5, 5);
  createRandomMatrix(numRows, numCols, &B, -5, 5);
  createRandomMatrix(numRows, numCols, &C, -5, 5);

  matrix_init_f32(&BC, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&AB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&AC, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABC1, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABC2, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Multiply and add matrices.
  matrix_add_f32(&B, &C, &BC);
  matrix_mult_f32(&A, &BC, &ABC1);

  matrix_mult_f32(&A, &B, &AB);
  matrix_mult_f32(&A, &C, &AC);
  matrix_add_f32(&AB, &AC, &ABC2);

  // Assert that distributive of matrix multiplication holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(ABC1.pData[i], ABC2.pData[i], MATRIX_MULT_PRECISION);
  }
}

/**
 * @brief Tests matrix multiplication identity property.
 * Property: AI = A.
 */
TEST(MatrixMultiplication, Identity) {
  const int numRows = 4, numCols = 4;
  matrix A, I, Result;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  createIdentityMatrix(numRows, &I);

  matrix_init_f32(&Result, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Multiply matrices.
  matrix_mult_f32(&A, &I, &Result);

  // Assert that identity of matrix multiplication holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(A.pData[i], Result.pData[i], MATRIX_MULT_PRECISION);
  }
}

/**
 * @brief Tests matrix multiplication zero property.
 * Property: A0 = 0.
 */
TEST(MatrixMultiplication, Zero) {
  const int numRows = 4, numCols = 4;
  matrix A, Zero, Result;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  matrix_init_f32(&Zero, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&Result, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Multiply matrices.
  matrix_mult_f32(&A, &Zero, &Result);

  // Assert that zeroing of matrix multiplication holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(Result.pData[i], 0.0f, MATRIX_MULT_PRECISION);
  }
}

/** @brief Test that matrix multiplication fails when matrices size are not
 * compatible. */
TEST(MatrixMultiplication, DimensionMismatch) {
  matrix A, B, C;

  // Create matrices.
  createRandomMatrix(4, 3, &A);
  createRandomMatrix(4, 3, &B);
  matrix_init_f32(&C, 4, 3, (float32_t*)calloc(9, sizeof(float32_t)));

  // Expect matrix multiplication failure due to incompatible matrix sizes.
  arm_status status = matrix_mult_f32(&A, &B, &C);
  ASSERT_EQ(status, ARM_MATH_SIZE_MISMATCH);
}
