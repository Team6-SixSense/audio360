/**
 ******************************************************************************
 * @file    performance_test.cpp
 * @brief   Performance tests for variable input signal sizes.
 *          Tests correspond to Test-NFR3.2 in VnVPlan.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <vector>

#include "constants.h"
#include "fft.h"

/**
 * @brief Generates a sine wave for performance testing.
 */
static std::vector<float> generateTestSignal(int numSamples,
                                             float frequency = 1000.0f,
                                             int sampleRate = 16000) {
  std::vector<float> signal(numSamples);
  for (int i = 0; i < numSamples; i++) {
    float t = static_cast<float>(i) / sampleRate;
    signal[i] = std::sin(TWO_PI_32 * frequency * t);
  }
  return signal;
}

/**
 * @brief Measures FFT processing time in milliseconds.
 */
static double measureProcessingTime(std::vector<float>& signal,
                                    int sampleRate) {
  auto start = std::chrono::high_resolution_clock::now();

  FFT fft(static_cast<uint16_t>(signal.size()), sampleRate);
  FrequencyDomain result;
  fft.signalToFrequency(signal.data(), result, WindowFunction::HANN_WINDOW);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;

  return duration.count();
}

/**
 * @brief Test-NFR3.2: Handle different input signal sizes.
 * @details Verifies that audio filtering processes various frame sizes
 *          within real-time constraints (16,000 samples/second).
 * @see VnVPlan.tex Section 3.2.3 (Audio Filtering Tests)
 */
TEST(PerformanceTest, VariableInputSizes_512) {
  const int sampleRate = 16000;
  const int numSamples = 512;

  std::vector<float> signal =
      generateTestSignal(numSamples, 1000.0f, sampleRate);
  double timeMs = measureProcessingTime(signal, sampleRate);

  // Calculate processing rate
  double processingRate = (numSamples / (timeMs / 1000.0));

  // Must process at least 16,000 samples per second
  EXPECT_GE(processingRate, 16000.0);

  std::cout << "  512 frames: " << timeMs << " ms"
            << " (" << processingRate << " samples/s)" << std::endl;
}

TEST(PerformanceTest, VariableInputSizes_1024) {
  const int sampleRate = 16000;
  const int numSamples = 1024;

  std::vector<float> signal =
      generateTestSignal(numSamples, 1000.0f, sampleRate);
  double timeMs = measureProcessingTime(signal, sampleRate);

  double processingRate = (numSamples / (timeMs / 1000.0));
  EXPECT_GE(processingRate, 16000.0);

  std::cout << "  1024 frames: " << timeMs << " ms"
            << " (" << processingRate << " samples/s)" << std::endl;
}

TEST(PerformanceTest, VariableInputSizes_2048) {
  const int sampleRate = 16000;
  const int numSamples = 2048;

  std::vector<float> signal =
      generateTestSignal(numSamples, 1000.0f, sampleRate);
  double timeMs = measureProcessingTime(signal, sampleRate);

  double processingRate = (numSamples / (timeMs / 1000.0));
  EXPECT_GE(processingRate, 16000.0);

  std::cout << "  2048 frames: " << timeMs << " ms"
            << " (" << processingRate << " samples/s)" << std::endl;
}

TEST(PerformanceTest, VariableInputSizes_4096) {
  const int sampleRate = 16000;
  const int numSamples = 4096;

  std::vector<float> signal =
      generateTestSignal(numSamples, 1000.0f, sampleRate);
  double timeMs = measureProcessingTime(signal, sampleRate);

  double processingRate = (numSamples / (timeMs / 1000.0));
  EXPECT_GE(processingRate, 16000.0);

  std::cout << "  4096 frames: " << timeMs << " ms"
            << " (" << processingRate << " samples/s)" << std::endl;
}

/**
 * @brief Verifies no buffer overflows during processing.
 */
TEST(PerformanceTest, NoBufferOverflows) {
  const int sampleRate = 16000;
  std::vector<int> sizes = {512, 1024, 2048, 4096};

  for (int size : sizes) {
    std::vector<float> signal = generateTestSignal(size, 1000.0f, sampleRate);

    // Processing should not throw or crash
    EXPECT_NO_THROW({
      FFT fft(static_cast<uint16_t>(signal.size()), sampleRate);
      FrequencyDomain result;

      fft.signalToFrequency(signal.data(), result, WindowFunction::HANN_WINDOW);
    });
  }
}
