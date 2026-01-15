/**
 ******************************************************************************
 * @file    constants.h
 * @brief   Project specific constants.
 ******************************************************************************
 */

#pragma once

#include <math.h>

#include <limits>

// Project specific constants.
constexpr inline int SAMPLE_FREQUENCY = 16000;
constexpr inline int WAVEFORM_SAMPLES = 4096;  // Must be power of 2.

// Math constants.
constexpr inline float FLOAT_EPS = std::numeric_limits<float>::epsilon();
constexpr inline float FLOAT_MAX = std::numeric_limits<float>::max();
constexpr inline float PI_32 = 3.14159265358979f;
constexpr inline float TWO_PI_32 = 2.0 * PI_32;

// Hardware constants.
constexpr inline float MIC1_2_DISTANCE_m = 0.10f;
constexpr inline float MIC2_3_DISTANCE_m = 0.10f;
constexpr inline float MIC3_4_DISTANCE_m = 0.10f;
constexpr inline float MIC4_1_DISTANCE_m = 0.10f;

// Physics constants.
constexpr inline float SOUND_AIR_mps = 343.0f;
