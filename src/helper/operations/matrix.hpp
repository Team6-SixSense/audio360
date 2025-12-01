/**
 ******************************************************************************
 * @file    matrix.hpp
 * @brief   Matrix operations wrapper code. This file is created to keep all
 *          implementation for ARM and non-ARM builds in one place.
 ******************************************************************************
 */

#pragma once

#include "arm_math.h"

/**
 * @brief Struct for representing matrices. This is used for matrix operation
 * since using CMSIS-DSP library.
 *
 * See @ref matrix_init_f32 for struct properties.
 *
 * https://arm-software.github.io/CMSIS_5/DSP/html/structarm__matrix__instance__f32.html
 */
typedef arm_matrix_instance_f32 matrix;

/**
 * @brief Initializes the matrix struct.
 *
 * @param[out] M Matrix instance
 * @param[in] nRows The number of rows in the matrix.
 * @param[in] nColumns The number of columns in the matrix.
 * @param[in] pData Points to matrix data array. Cell (i, j) is stored in
 * pData[i*numCols + j]
 */
void matrix_init_f32(matrix* M, uint16_t nRows, uint16_t nColumns,
                     float32_t* pData) {
  arm_mat_init_f32(M, nRows, nColumns, pData);
}

/**
 * @brief Add two matrices. A + B = C
 *
 * @param[in] pSrcA Matrix A.
 * @param[in] pSrcB Matrix B.
 * @param[out] pDst Matrix C.
 * @return arm_status The status of the operation.
 */
arm_status matrix_add_f32(const matrix* pSrcA, const matrix* pSrcB,
                          matrix* pDst)

{
  return arm_mat_add_f32(pSrcA, pSrcB, pDst);
}

/**
 * @brief Subtracts two matrices. A - B = C.
 *
 * @param[in] pSrcB Matrix B.
 * @param[in] pSrcA Matrix A.
 * @param[out] pDst Matrix C.
 * @return arm_status The status of the operation.
 */
arm_status matrix_sub_f32(const matrix* pSrcA, const matrix* pSrcB,
                          matrix* pDst) {
  return arm_mat_sub_f32(pSrcA, pSrcB, pDst);
}

/**
 * @brief Scales matrix by amplitude c. cA = B
 *
 * @param[in] pSrc Matrix A.
 * @param[in] scale Scalar c.
 * @param[out] pDst Matrix B.
 * @return arm_status The status of the operation.
 */
arm_status matrix_scale_f32(const matrix* pSrc, float32_t scale, matrix* pDst) {
  return arm_mat_scale_f32(pSrc, scale, pDst);
}

/**
 * @brief Multiplies two matrices. A * B = C
 *
 * @param[in] pSrcA Matrix A.
 * @param[in] pSrcB Matrix B.
 * @param[out] pDst Matrix C.
 * @return arm_status The status of the operation.
 */
arm_status matrix_mult_f32(const matrix* pSrcA, const matrix* pSrcB,
                           matrix* pDst) {
  return arm_mat_mult_f32(pSrcA, pSrcB, pDst);
}

/**
 * @brief Transposes the input matrix.
 *
 * @param[in] pSrc Input matrix.
 * @param[out] pDst Transposed matrix.
 * @return arm_status The status of the operation.
 */
arm_status matrix_transpose_f32(const matrix* pSrc, matrix* pDst) {
  return arm_mat_trans_f32(pSrc, pDst);
}

/**
 * @brief Determines the inverse of input matrix.
 *
 * @param[in] pSrc Input matrix.
 * @param[out] pDst Inversed matrix.
 * @return arm_status The status of the operation.
 */
arm_status matrix_inverse_f32(const matrix* pSrc, matrix* pDst) {
  return arm_mat_inverse_f32(pSrc, pDst);
}
