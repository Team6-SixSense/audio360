/**
 ******************************************************************************
 * @file    matrix.cpp
 * @brief   Matrix operations wrapper source code. This file is created to keep
 * all implementation for ARM and non-ARM builds in one place.
 ******************************************************************************
 */

#pragma once

#include "matrix.h"

#include "arm_math.h"

void matrix_init_f32(matrix* M, uint16_t nRows, uint16_t nColumns,
                     float32_t* pData) {
  arm_mat_init_f32(M, nRows, nColumns, pData);
}

arm_status matrix_add_f32(const matrix* pSrcA, const matrix* pSrcB,
                          matrix* pDst)

{
  return arm_mat_add_f32(pSrcA, pSrcB, pDst);
}

arm_status matrix_sub_f32(const matrix* pSrcA, const matrix* pSrcB,
                          matrix* pDst) {
  return arm_mat_sub_f32(pSrcA, pSrcB, pDst);
}

arm_status matrix_scale_f32(const matrix* pSrc, float32_t scale, matrix* pDst) {
  return arm_mat_scale_f32(pSrc, scale, pDst);
}

arm_status matrix_mult_f32(const matrix* pSrcA, const matrix* pSrcB,
                           matrix* pDst) {
  return arm_mat_mult_f32(pSrcA, pSrcB, pDst);
}

arm_status matrix_transpose_f32(const matrix* pSrc, matrix* pDst) {
  return arm_mat_trans_f32(pSrc, pDst);
}

arm_status matrix_inverse_f32(const matrix* pSrc, matrix* pDst) {
  return arm_mat_inverse_f32(pSrc, pDst);
}
