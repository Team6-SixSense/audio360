/**
 ******************************************************************************
 * @file    classification_constants.h
 * @brief   Classification-specific constants for PCA and LDA.
 ******************************************************************************
 */

#pragma once

#include <string>
#include <vector>

#include "matrix.h"

inline std::vector<float> PCA_MEAN_VECTOR = {
  6.46898222, 2.60436964, -0.85070765, 0.06396702, -0.10794854, 0.23375654
};

inline std::vector<float> PCA_PROJECTION_MATRIX_DATA = {
  0.99559802, -0.09239442, 0.01102169, -0.00301231, -0.00956578, -0.00507632, 0.09004891, 0.96183860, -0.20649843, -0.09744891, -0.12058855, -0.00884302, 0.01369788,
  0.23794189, 0.89495516, 0.35119614, 0.09877268, -0.09568963, 0.01895922, 0.07096004, -0.34981528, 0.70586139, 0.60014576, 0.11755566, 0.01126786, 0.06839648,
  0.16843688, -0.58750129, 0.71609241, 0.32996649, 0.00130328, -0.00011290, 0.07450140, 0.15413588, -0.32038671, 0.93168789
};

inline matrix PCA_PROJECTION_MATRIX = {6, 6,
                                       PCA_PROJECTION_MATRIX_DATA.data()};

inline std::vector<float> LDA_CLASS_WEIGHTS_DATA = {
  0.00257889, -0.05344952, 0.07298273, 0.24190538, -1.21824944, 0.84361267, 0.09902324, -0.53878903, 0.65052789, -0.23960622, 0.73106378, -0.30330455, -0.09339298,
  0.53323573, -0.64919847, 0.06809019, 0.09777755, -0.25574917
};

inline matrix LDA_CLASS_WEIGHTS = {3, 6, LDA_CLASS_WEIGHTS_DATA.data()};

inline std::vector<float> LDA_CLASS_BIASES = {
  -2.14772081, -1.91962731, -1.82431436
};

inline std::vector<std::string> CLASSIFICATION_CLASSES = {
    "car_horn", "jackhammer", "siren"};
