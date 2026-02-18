/**
 ******************************************************************************
 * @file    classification_constants.h
 * @brief   Classification-specific constants for PCA and LDA.
 ******************************************************************************
 */

#pragma once

#include <string>
#include <vector>

#include "classificationLabel.h"
#include "matrix.h"

extern std::vector<float> PCA_MEAN_VECTOR;

extern std::vector<float> PCA_PROJECTION_MATRIX_DATA;

extern matrix PCA_PROJECTION_MATRIX;

extern std::vector<float> LDA_CLASS_WEIGHTS_DATA;

extern matrix LDA_CLASS_WEIGHTS;

extern std::vector<float> LDA_CLASS_BIASES;

extern std::vector<float> LDA_SCALINGS_DATA;

// 6 rows (PCA dims), 2 cols (C-1)
extern matrix LDA_SCALINGS;

extern std::vector<ClassificationLabel> CLASSIFICATION_CLASSES;
