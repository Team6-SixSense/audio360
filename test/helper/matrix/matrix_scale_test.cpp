/**
 ******************************************************************************
 * @file    matrix_scale_test.cpp
 * @brief   Unit tests for matrix scaling.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include "matrix_test.h"

/**
 *  @brief Tests matrix scaling associative property holds.
 * Property: (cd)A = c(dA).
 */
TEST(MatrixScale, Associative) {
  const int numRows = 4, numCols = 4;
  const int c = 2, d = 3;
  matrix A, cdA1, cdA2;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A, -5, 5);

  matrix_init_f32(&cdA1, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cdA2, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Scale matrix.
  matrix_scale_f32(&A, c * d, &cdA1);

  matrix_scale_f32(&A, d, &cdA2);
  matrix_scale_f32(&cdA2, c, &cdA2);

  // Assert that matrix scaling associative property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(cdA1.pData[i], cdA2.pData[i], MATRIX_MULT_PRECISION);
  }
}

/**
 *  @brief Tests matrix scaling distributive property holds.
 * Property: c(A + B) = cA + cB.
 */
TEST(MatrixScale, Distributive1) {
  const int numRows = 4, numCols = 4;
  const int c = 2;
  matrix A, B, AB, cAB, cA, cB, cAcB;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A, -5, 5);
  createRandomMatrix(numRows, numCols, &B, -5, 5);

  matrix_init_f32(&AB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cAB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cA, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cAcB, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Scale and add matrices.
  matrix_add_f32(&A, &B, &AB);
  matrix_scale_f32(&AB, c, &cAB);

  matrix_scale_f32(&A, c, &cA);
  matrix_scale_f32(&B, c, &cB);
  matrix_add_f32(&cA, &cB, &cAcB);

  // Assert that matrix scaling distributive property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(cAB.pData[i], cAcB.pData[i], MATRIX_MULT_PRECISION);
  }
}

/**
 *  @brief Tests matrix scaling distributive property holds.
 * Property: (c+d)A = cA + dA.
 */
TEST(MatrixScale, Distributive2) {
  const int numRows = 4, numCols = 4;
  const int c = 2, d = 3;
  matrix A, cdA, cA, dA, cAdA;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A, -5, 5);

  matrix_init_f32(&cdA, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cA, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&dA, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));
  matrix_init_f32(&cAdA, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Scale and add matrices.
  matrix_scale_f32(&A, c + d, &cdA);

  matrix_scale_f32(&A, c, &cA);
  matrix_scale_f32(&A, d, &dA);
  matrix_add_f32(&cA, &dA, &cAdA);

  // Assert that matrix scaling distributive property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(cdA.pData[i], cAdA.pData[i], MATRIX_MULT_PRECISION);
  }
}

/**
 *  @brief Tests matrix scaling identity property holds.
 * Property: A*1 = A.
 */
TEST(MatrixScale, Identity) {
  const int numRows = 4, numCols = 4;
  matrix A, Result;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);

  matrix_init_f32(&Result, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Scale matrix.
  matrix_scale_f32(&A, 1, &Result);

  // Assert that matrix scaling identity property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(Result.pData[i], A.pData[i], MATRIX_MULT_PRECISION);
  }
}

/**
 *  @brief Tests matrix zero scaling property holds
 * Property: A*0 = 0.
 */
TEST(MatrixScale, Zero) {
  const int numRows = 4, numCols = 4;
  matrix A, Result;

  // Create matrices.
  createRandomMatrix(numRows, numCols, &A);
  matrix_init_f32(&Result, numRows, numCols,
                  (float32_t*)calloc(numRows * numCols, sizeof(float32_t)));

  // Scale matrix.
  matrix_scale_f32(&A, 0, &Result);

  // Assert that the matrix inverse property holds.
  for (int i = 0; i < numRows * numCols; i++) {
    ASSERT_NEAR(Result.pData[i], 0.0f, MATRIX_MULT_PRECISION);
  }
}
