/**
 ******************************************************************************
 * @file    constants.h
 * @brief   Project specific constants.
 ******************************************************************************
 */

#pragma once

#include <math.h>

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "matrix.h"

// Project specific constants.
constexpr inline int SAMPLE_FREQUENCY = 16000;
constexpr inline int WAVEFORM_SAMPLES = 4096;  // Must be power of 2.
constexpr inline int DOA_SAMPLES = 2048;       // Must be power of 2.
constexpr inline size_t DIRECTION_MODE_FILTER_SIZE = 3;
constexpr inline size_t CLASSIFICATION_MODE_FILTER_SIZE = 3;
constexpr inline int CLASSIFICATION_BUFFER_SIZE = 8;

constexpr inline float CONFIDENCE_THRESHOLD = 0.9f;

// Math constants.
constexpr inline float FLOAT_EPS = std::numeric_limits<float>::epsilon();
constexpr inline float FLOAT_MAX = std::numeric_limits<float>::max();
constexpr inline float PI_32 = 3.14159265358979f;
constexpr inline float TWO_PI_32 = 2.0 * PI_32;

// Hardware constants.
#ifndef BUILD_TESTS
#ifdef PCB_BUILD
constexpr inline float MIC1_2_DISTANCE_m = 0.134f;
constexpr inline float MIC2_3_DISTANCE_m = 0.13f;
constexpr inline float MIC3_4_DISTANCE_m = 0.134f;
constexpr inline float MIC4_1_DISTANCE_m = 0.13f;
#else
// Rev 0 build
constexpr inline float MIC1_2_DISTANCE_m = 0.14f;
constexpr inline float MIC2_3_DISTANCE_m = 0.127f;
constexpr inline float MIC3_4_DISTANCE_m = 0.14f;
constexpr inline float MIC4_1_DISTANCE_m = 0.127f;
#endif  // PCB_BUILD
#else
constexpr inline float MIC1_2_DISTANCE_m = 0.10f;
constexpr inline float MIC2_3_DISTANCE_m = 0.10f;
constexpr inline float MIC3_4_DISTANCE_m = 0.10f;
constexpr inline float MIC4_1_DISTANCE_m = 0.10f;
#endif  // BUILD_TESTS

// 24 bit minimum: -2^23
constexpr inline int32_t MIN_AUDIO_SAMPLE_DATA = -8388608;

// 24 bit maximum: 2^23-1
constexpr inline int32_t MAX_AUDIO_SAMPLE_DATA = 8388607;

constexpr inline uint8_t BLUTOOTH_CONNECTED = 1U;

// Physics constants.
constexpr inline float SOUND_AIR_mps = 343.0f;
