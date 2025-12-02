/**
 ******************************************************************************
 * @file    matrix_test.cpp
 * @brief   Helper functions source code for matrix tests.
 ******************************************************************************
 */

#include "matrix_test.h"

void createIdentityMatrix(int n, matrix* M) {
  int size = n * n;
  float32_t* pData = (float32_t*)calloc(size, sizeof(float32_t));

  for (int i = 0; i < n; i++) {
    pData[i * n + i] = 1;
  }

  matrix_init_f32(M, n, n, pData);
}

void createRandomMatrix(int numRows, int numCols, matrix* M, float32_t minVal,
                        float32_t maxVal) {
  int size = numRows * numCols;
  float32_t* pData = (float32_t*)calloc(size, sizeof(float32_t));

  for (int i = 0; i < size; i++) {
    pData[i] = generateRandomFloat32();
  }

  matrix_init_f32(M, numRows, numCols, pData);
}
