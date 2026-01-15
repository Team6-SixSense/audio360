/**
 ******************************************************************************
 * @file    gccPhat.h
 * @brief   Generalized Cross-Correlation with Phase Transform (GCC-PhaT)
 *          header.
 ******************************************************************************
 */

#pragma once

#include <vector>

#include "doaAlgorithm.h"
#include "fft.h"
#include "frequencyDomain.h"
#include "ifft.h"

/** @brief Module to handle GCC-PhaT DOA algo. */
class GCCPhaT : DoAAlgo {
 public:
  /**
   * @brief Construct a new GCCPhaT object.
   *
   * @param numSamples Number of samples that will be processed from each input
   * source.
   */
  GCCPhaT(size_t numSamples);

  /**
   * @brief Calculate the direction of the audio source.
   *
   * @param mic1Data Microphone 1 audio data.
   * @param mic2Data Microphone 2 audio data.
   * @param mic3Data Microphone 3 audio data.
   * @param mic4Data Microphone 4 audio data.
   * @return float Angle of audio source in radian.
   */
  float calculateDirection(std::vector<float>& mic1Data,
                           std::vector<float>& mic2Data,
                           std::vector<float>& mic3Data,
                           std::vector<float>& mic4Data) override;

 private:
  /**
   * @brief Compute the time delay between two audio sources.
   *
   * @param freqA Frequency domain of an audio source.
   * @param freqB Frequency domain of a different audio source.
   * @param maxDelay_s The maximum physically allowed time delay between the two
   * audio sources.
   * @return float The time delay of audio signal in seconds.
   */
  float computeTimeDelay2Source(const FrequencyDomain& freqA,
                                const FrequencyDomain& freqB, float maxDelay_s);

  /**
   * @brief Compute the GCC PhaT frequency domain.
   *
   * @param freqA Frequency domain of an audio source.
   * @param freqB Frequency domain of a different audio source.
   */
  void computeGCCPhaT(const FrequencyDomain& freqA,
                      const FrequencyDomain& freqB);

  /**
   * @brief Calculate the time delay from the peaks of the GCC PhaT time domain.
   *
   * @param timeDomain The GCC PhaT in the time domain.
   * @param maxDelay_s The maximum physically allowed time delay between the two
   * audio sources.
   * @return float The time delay of audio signal in seconds.
   */
  float calculateTimeDelay(const std::vector<float>& timeDomain,
                           float maxDelay_s);

  /**
   * @brief Estimate the angle of direction in radian.
   *
   * @param timeDelayX1 Time delay on x axis from 2 audio sources.
   * @param timeDelayX2 Time delay on x axis from 2 different pairs of audio
   * sources.
   * @param timeDelayY1 Time delay on y axis from 2 audio sources.
   * @param timeDelayY2 Time delay on y axis from 2 different pairs of audio
   * sources.
   * @return float The angle of the audio source in radians.
   */
  float estimateAngle(float timeDelayX1, float timeDelayX2, float timeDelayY1,
                      float timeDelayY2);

  /** @brief The number of samples of input audio source data. */
  size_t numSamples{0};

  /** @brief GCC PhaT frequency domain. */
  FrequencyDomain gccPhatFreqDomain;

  /** @brief Fast Fourier Transform (FFT) instance. */
  FFT fft;

  /** @brief Inverse Fast Fourier Transform (IFFT) instance. */
  IFFT ifft;
};
