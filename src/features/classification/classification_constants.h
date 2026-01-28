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

inline std::vector<float> PCA_MEAN_VECTOR = {
  9.94755363, 2.80622458, -1.22489440, -0.15745239, -0.17760618, 0.24383263
};

inline std::vector<float> PCA_PROJECTION_MATRIX_DATA = {
  0.99450815, -0.10281198, 0.01165613, -0.00188354, -0.01290235, -0.00879581, 0.10118255, 0.96754146, -0.19424100, -0.08988219, -0.08626934, 0.01936027, 0.01585529,
  0.22675711, 0.88002598, 0.37235996, 0.18333150, -0.04026981, 0.01733513, 0.02617091, -0.41429085, 0.71510667, 0.53809595, 0.16264352, 0.01279059, 0.02420260,
  0.09847789, -0.57627505, 0.73967010, 0.33218667, 0.00038661, -0.02457091, 0.07971204, 0.09897089, -0.34945902, 0.92796832
};

inline matrix PCA_PROJECTION_MATRIX = {6, 6,
                                       PCA_PROJECTION_MATRIX_DATA.data()};

inline std::vector<float> LDA_CLASS_WEIGHTS_DATA = {0.03074151,  1.2104454, -1.5900505, -0.41783714, 1.4128469 , -0.44179523};

inline matrix LDA_CLASS_WEIGHTS = {3, 2, LDA_CLASS_WEIGHTS_DATA.data()};

inline std::vector<float> LDA_CLASS_BIASES = {
  3.528, -9.737268, 7.043996
};

inline std::vector<float> LDA_SCALINGS_DATA = {
  -0.07936350, -0.01101666,
   0.38999072, -0.04274665,
  -0.55441391,  0.00280420,
   0.21863136,  0.12364489,
  -0.07492033, -1.26101398,
   0.04050463,  0.37705261
};

// 6 rows (PCA dims), 2 cols (C-1)
inline matrix LDA_SCALINGS = {6, 2, LDA_SCALINGS_DATA.data()};

inline std::vector<ClassificationLabel> CLASSIFICATION_CLASSES = {
    ClassificationLabel::Jackhammer, ClassificationLabel::CarHorn, ClassificationLabel::Siren};
