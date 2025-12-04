/**
 ******************************************************************************
 * @file    matrix_transpose_test.cpp
 * @brief   Unit tests for matrix transpose.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "matrix_test.h"

/**
 * @brief Tests matrix transpose involution property.
 * Property: (A^T)^T = A.
 */
TEST(MatrixTranspose, Involution) {
  const int numRows = 4, numCols = 4;
  matrix A, AT, ATT;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);

  matrix_init_f32(&AT, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ATT, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Transpose matrices.
  matrix_transpose_f32(&A, &AT);
  matrix_transpose_f32(&AT, &ATT);

  // Assert that involution of matrix transpose holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(ATT.pData[i], A.pData[i], PRECISION_ERROR);
  }
}

/**
 * @brief Tests matrix transpose addition property.
 * Property: (A + B)^T = A^T + B^T.
 */
TEST(MatrixTranspose, Addition) {
  const int numRows = 4, numCols = 4;
  matrix A, B, AB, AT, BT, ABT1, ABT2;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  createRandomMatrix(numRows, numCols, &B);

  matrix_init_f32(&AB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&AT, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&BT, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABT1, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABT2, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Transpose and add matrices.
  matrix_add_f32(&A, &B, &AB);
  matrix_transpose_f32(&AB, &ABT1);

  matrix_transpose_f32(&A, &AT);
  matrix_transpose_f32(&B, &BT);
  matrix_add_f32(&AT, &BT, &ABT2);

  // Assert that addition of matrix transpose holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(ABT1.pData[i], ABT2.pData[i], PRECISION_ERROR);
  }
}

/**
 * @brief Tests matrix transpose scalar property.
 * Property: (cA)^T = cA^T.
 */
TEST(MatrixTranspose, Scalar) {
  const int numRows = 4, numCols = 4;
  const int c = 2;
  matrix A, cA, AT, cAT1, cAT2;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);

  matrix_init_f32(&cA, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&AT, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cAT1, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cAT2, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Transpose and scale matrices.
  matrix_scale_f32(&A, c, &cA);
  matrix_transpose_f32(&cA, &cAT1);

  matrix_transpose_f32(&A, &AT);
  matrix_scale_f32(&AT, c, &cAT2);

  // Assert that scalar of matrix transpose holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(cAT1.pData[i], cAT2.pData[i], PRECISION_ERROR);
  }
}

/**
 * @brief Tests matrix transpose multiplication property.
 * Property: (AB)^T = B^TA^T.
 */
TEST(MatrixTranspose, Multiplication) {
  const int numRows = 4, numCols = 4;
  matrix A, B, AB, BT, AT, ABT1, ABT2;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  createRandomMatrix(numRows, numCols, &B);

  matrix_init_f32(&AB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&BT, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&AT, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABT1, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&ABT2, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Transpose and multiply matrices.
  matrix_mult_f32(&A, &B, &AB);
  matrix_transpose_f32(&AB, &ABT1);

  matrix_transpose_f32(&A, &AT);
  matrix_transpose_f32(&B, &BT);
  matrix_mult_f32(&BT, &AT, &ABT2);

  // Assert that multiplication of matrix transpose holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(ABT1.pData[i], ABT2.pData[i], PRECISION_ERROR);
  }
}
