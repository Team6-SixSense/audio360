/**
 ******************************************************************************
 * @file    runtime_audio360.hpp
 * @brief   FFT runtime code.
 ******************************************************************************
 */

#pragma once

#include "constants.h"

const int MIC_BUFFER_SIZE = 4096;
const int MIC_HALF_BUFFER_SIZE = WAVEFORM_SAMPLES / 2;
const int MIC_CLASSIFICATION_BUFFER_SIZE = MIC_BUFFER_SIZE * 4;

const int NUM_MEL_FILTERS = 6;
const int NUM_DCT_COEFF = 6;
const int NUM_PCA_COMPONENTS = 6;
const int NUM_CLASSES = 3;

/** @brief Main entry code. */
void mainAudio360();

/**
 * @brief Extract mic data from dynamic memory when ready and store in a
 * larger buffer.
 *
 * Assumption: Since microphones are synchronized on the same clock, when mic A1
 * (master) has its data read, all other microphones have their data ready.
 *
 * @return bool: True if there are new microphone data.
 */
bool extractMicData();

/**
 * @brief Run Direction of Arrival feature.
 *
 * @param newData True if there is new microphone data in the buffer.
 * @return float Angle of audio source in radian.
 */
float runDoA(bool newData);

std::string runClassification(bool newData);
