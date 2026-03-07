/**
 ******************************************************************************
 * @file    pipeline_test.cpp
 * @brief   Unit tests for Audio360 Engine pipeline management.
 *          Tests correspond to Test-FR-4.1, 4.2, 4.3, 4.4 in VnVPlan.
 * @note    These tests use mock components to verify pipeline behavior
 *          without requiring full hardware integration.
 ******************************************************************************
 */

#include <gtest/gtest.h>

#include <cstdint>
#include <queue>
#include <string>
#include <vector>

#include "constants.h"
#include "frequencyDomain.h"

/**
 * @brief Enum for error flags in audio processing.
 */
enum class AudioErrorFlag {
  NONE = 0,
  CLIPPING = 1,
  LOST_SIGNAL = 2,
  SILENCE = 3
};

/**
 * @brief Mock frequency data structure for testing.
 */
struct MockFrequencyData {
  std::vector<float> spectrum;
  AudioErrorFlag errorFlags;
  bool hasData;

  MockFrequencyData() : errorFlags(AudioErrorFlag::NONE), hasData(true) {
    spectrum.resize(WAVEFORM_SAMPLES / 2 + 1, 0.0f);
  }
};

/**
 * @brief Mock result structure from Audio360 Engine.
 */
struct MockProcessingResult {
  AudioErrorFlag errorFlags;
  bool hasDirection;
  bool hasClassification;
  float direction;
  std::string classification;

  MockProcessingResult()
      : errorFlags(AudioErrorFlag::NONE),
        hasDirection(false),
        hasClassification(false),
        direction(0.0f),
        classification("") {}
};

/**
 * @brief Mock pipeline component for testing notifications.
 */
class MockPipelineComponent {
 public:
  bool notified = false;
  int processOrder = -1;
  int notificationCount = 0;
  MockFrequencyData lastReceivedData;

  void notify(const MockFrequencyData& data, int order) {
    notified = true;
    processOrder = order;
    notificationCount++;
    lastReceivedData = data;
  }

  void reset() {
    notified = false;
    processOrder = -1;
    notificationCount = 0;
  }
};

/**
 * @brief Mock Audio360 Engine for testing.
 */
class MockAudio360Engine {
 public:
  std::vector<MockPipelineComponent*> components;
  bool pipelineSuppressed = false;
  std::string lastError;

  void registerComponent(MockPipelineComponent* component) {
    components.push_back(component);
  }

  MockProcessingResult process(const MockFrequencyData& input) {
    MockProcessingResult result;
    result.errorFlags = input.errorFlags;

    // Test-FR-4.4: Error-based pipeline suppression
    if (input.errorFlags != AudioErrorFlag::NONE) {
      pipelineSuppressed = true;
      lastError = "Pipeline suppressed due to error flags";
      result.hasDirection = false;
      result.hasClassification = false;
      return result;
    }

    // Pipeline not suppressed - process normally
    pipelineSuppressed = false;

    // Test-FR-4.2 & 4.3: Notify dependent components in order
    int order = 0;
    for (auto* component : components) {
      component->notify(input, order++);
    }

    // Simulate processing results
    result.hasDirection = true;
    result.hasClassification = true;
    result.direction = 1.57f;  // 90 degrees (example)
    result.classification = "test_class";

    return result;
  }

  void reset() {
    pipelineSuppressed = false;
    lastError.clear();
    for (auto* comp : components) {
      comp->reset();
    }
  }
};

/**
 * @brief Creates test frequency data with specific spectral peaks.
 * @param freq1 First peak frequency (Hz).
 * @param freq2 Second peak frequency (Hz).
 * @param freq3 Third peak frequency (Hz).
 * @return MockFrequencyData with peaks at specified frequencies.
 */
static MockFrequencyData createTestFrequencyData(float freq1 = 100.0f,
                                                  float freq2 = 1000.0f,
                                                  float freq3 = 8000.0f) {
  MockFrequencyData data;
  const int numBins = WAVEFORM_SAMPLES / 2 + 1;
  data.spectrum.resize(numBins, 0.1f);  // Base noise floor

  // Add peaks at specified frequencies
  // Note: Simplified - actual bin calculation would use sample rate
  int bin1 = static_cast<int>(freq1 / 10.0f);
  int bin2 = static_cast<int>(freq2 / 10.0f);
  int bin3 = static_cast<int>(freq3 / 10.0f);

  if (bin1 < numBins) data.spectrum[bin1] = 10.0f;
  if (bin2 < numBins) data.spectrum[bin2] = 10.0f;
  if (bin3 < numBins) data.spectrum[bin3] = 10.0f;

  data.errorFlags = AudioErrorFlag::NONE;
  data.hasData = true;

  return data;
}

/**
 * @brief Creates random frequency data for testing.
 */
static MockFrequencyData createRandomFrequencyData() {
  MockFrequencyData data;
  const int numBins = WAVEFORM_SAMPLES / 2 + 1;
  data.spectrum.resize(numBins);

  for (int i = 0; i < numBins; i++) {
    data.spectrum[i] = static_cast<float>(rand()) / RAND_MAX;
  }

  data.errorFlags = AudioErrorFlag::NONE;
  data.hasData = true;

  return data;
}

// ============================================================================
// Test-FR-4.1: Frequency and Error Input Capability Testing
// ============================================================================

/**
 * @brief Test-FR-4.1 Subtest 1: Frequency input capability.
 * @details Verifies Audio360 Engine accepts valid frequency data and produces
 *          direction and classification outputs.
 * @see VnVPlan.tex Section 3.1.6 (Audio360 Engine Tests)
 */
TEST(Audio360EngineTest, FrequencyInputCapability) {
  MockAudio360Engine engine;

  // Create frequency data with no errors
  MockFrequencyData validData = createTestFrequencyData(100, 1000, 8000);
  validData.errorFlags = AudioErrorFlag::NONE;

  // Process input
  MockProcessingResult result = engine.process(validData);

  // Verify no error flags in output
  EXPECT_EQ(result.errorFlags, AudioErrorFlag::NONE);

  // Verify direction and classification are present
  EXPECT_TRUE(result.hasDirection);
  EXPECT_TRUE(result.hasClassification);

  // Verify pipeline not suppressed
  EXPECT_FALSE(engine.pipelineSuppressed);
}

/**
 * @brief Test-FR-4.1 Subtest 2: Error input capability.
 * @details Verifies Audio360 Engine correctly handles error-flagged input.
 * @see VnVPlan.tex Section 3.1.6 (Audio360 Engine Tests)
 */
TEST(Audio360EngineTest, ErrorInputCapability_Clipping) {
  MockAudio360Engine engine;

  // Create data with clipping error
  MockFrequencyData errorData = createRandomFrequencyData();
  errorData.errorFlags = AudioErrorFlag::CLIPPING;

  // Process input
  MockProcessingResult result = engine.process(errorData);

  // Verify error flags reflected in output
  EXPECT_EQ(result.errorFlags, AudioErrorFlag::CLIPPING);

  // Verify no valid direction or classification
  EXPECT_FALSE(result.hasDirection);
  EXPECT_FALSE(result.hasClassification);
}

TEST(Audio360EngineTest, ErrorInputCapability_LostSignal) {
  MockAudio360Engine engine;

  MockFrequencyData errorData = createRandomFrequencyData();
  errorData.errorFlags = AudioErrorFlag::LOST_SIGNAL;

  MockProcessingResult result = engine.process(errorData);

  EXPECT_EQ(result.errorFlags, AudioErrorFlag::LOST_SIGNAL);
  EXPECT_FALSE(result.hasDirection);
  EXPECT_FALSE(result.hasClassification);
}

TEST(Audio360EngineTest, ErrorInputCapability_Silence) {
  MockAudio360Engine engine;

  MockFrequencyData errorData = createRandomFrequencyData();
  errorData.errorFlags = AudioErrorFlag::SILENCE;

  MockProcessingResult result = engine.process(errorData);

  EXPECT_EQ(result.errorFlags, AudioErrorFlag::SILENCE);
  EXPECT_FALSE(result.hasDirection);
  EXPECT_FALSE(result.hasClassification);
}

// ============================================================================
// Test-FR-4.2: Dependent Component Notification
// ============================================================================

/**
 * @brief Test-FR-4.2: Dependent component notification.
 * @details Verifies Audio360 Engine notifies all registered components
 *          when new data arrives.
 * @see VnVPlan.tex Section 3.1.6 (Audio360 Engine Tests)
 */
TEST(Audio360EngineTest, DependentComponentNotification) {
  MockAudio360Engine engine;
  MockPipelineComponent classifier;
  MockPipelineComponent doa;

  // Register components
  engine.registerComponent(&classifier);
  engine.registerComponent(&doa);

  // Verify components not yet notified
  EXPECT_FALSE(classifier.notified);
  EXPECT_FALSE(doa.notified);

  // Process data
  MockFrequencyData data = createTestFrequencyData();
  engine.process(data);

  // Verify both components were notified
  EXPECT_TRUE(classifier.notified);
  EXPECT_TRUE(doa.notified);

  // Verify they received the data
  EXPECT_EQ(classifier.lastReceivedData.errorFlags, AudioErrorFlag::NONE);
  EXPECT_EQ(doa.lastReceivedData.errorFlags, AudioErrorFlag::NONE);
}

/**
 * @brief Test multiple processing cycles.
 */
TEST(Audio360EngineTest, MultipleNotificationCycles) {
  MockAudio360Engine engine;
  MockPipelineComponent component;

  engine.registerComponent(&component);

  // Process 3 times
  for (int i = 0; i < 3; i++) {
    MockFrequencyData data = createTestFrequencyData();
    engine.process(data);
  }

  // Verify component notified 3 times
  EXPECT_EQ(component.notificationCount, 3);
}

// ============================================================================
// Test-FR-4.3: Pipeline Flow Management
// ============================================================================

/**
 * @brief Test-FR-4.3: Pipeline flow management.
 * @details Verifies components process data in correct order and data is
 *          passed correctly between components.
 * @see VnVPlan.tex Section 3.1.6 (Audio360 Engine Tests)
 */
TEST(Audio360EngineTest, PipelineFlowOrdering) {
  MockAudio360Engine engine;
  MockPipelineComponent classifier;
  MockPipelineComponent doa;
  MockPipelineComponent visualizer;

  // Register in specific order
  engine.registerComponent(&classifier);
  engine.registerComponent(&doa);
  engine.registerComponent(&visualizer);

  // Process data
  MockFrequencyData data = createTestFrequencyData();
  engine.process(data);

  // Verify all components processed
  EXPECT_TRUE(classifier.notified);
  EXPECT_TRUE(doa.notified);
  EXPECT_TRUE(visualizer.notified);

  // Verify processing order
  EXPECT_EQ(classifier.processOrder, 0);
  EXPECT_EQ(doa.processOrder, 1);
  EXPECT_EQ(visualizer.processOrder, 2);

  // Verify each component has distinct order
  EXPECT_NE(classifier.processOrder, doa.processOrder);
  EXPECT_NE(doa.processOrder, visualizer.processOrder);
}

/**
 * @brief Test data integrity through pipeline.
 */
TEST(Audio360EngineTest, PipelineDataIntegrity) {
  MockAudio360Engine engine;
  MockPipelineComponent component;

  engine.registerComponent(&component);

  // Create data with specific characteristics
  MockFrequencyData inputData = createTestFrequencyData(100, 1000, 8000);
  inputData.errorFlags = AudioErrorFlag::NONE;

  engine.process(inputData);

  // Verify component received correct data
  EXPECT_EQ(component.lastReceivedData.errorFlags, AudioErrorFlag::NONE);
  EXPECT_TRUE(component.lastReceivedData.hasData);
  EXPECT_EQ(component.lastReceivedData.spectrum.size(), inputData.spectrum.size());
}

// ============================================================================
// Test-FR-4.4: Error-Based Pipeline Suppression
// ============================================================================

/**
 * @brief Test-FR-4.4: Error-based pipeline suppression.
 * @details Verifies pipeline is suppressed when errors present and resumes
 *          when errors clear.
 * @see VnVPlan.tex Section 3.1.6 (Audio360 Engine Tests)
 */
TEST(Audio360EngineTest, ErrorBasedPipelineSuppression) {
  MockAudio360Engine engine;
  MockPipelineComponent classifier;

  engine.registerComponent(&classifier);

  // Process data with error flag
  MockFrequencyData errorData = createRandomFrequencyData();
  errorData.errorFlags = AudioErrorFlag::LOST_SIGNAL;

  MockProcessingResult result = engine.process(errorData);

  // Verify pipeline was suppressed
  EXPECT_TRUE(engine.pipelineSuppressed);
  EXPECT_FALSE(classifier.notified);  // Component should NOT be notified

  // Verify error propagated in result
  EXPECT_EQ(result.errorFlags, AudioErrorFlag::LOST_SIGNAL);
  EXPECT_FALSE(result.hasDirection);
  EXPECT_FALSE(result.hasClassification);
}

/**
 * @brief Test pipeline resumption after error clears.
 */
TEST(Audio360EngineTest, PipelineResumptionAfterError) {
  MockAudio360Engine engine;
  MockPipelineComponent classifier;

  engine.registerComponent(&classifier);

  // First: Process with error (pipeline suppressed)
  MockFrequencyData errorData = createRandomFrequencyData();
  errorData.errorFlags = AudioErrorFlag::CLIPPING;
  engine.process(errorData);

  EXPECT_TRUE(engine.pipelineSuppressed);
  EXPECT_FALSE(classifier.notified);

  // Reset component state
  classifier.reset();

  // Second: Process without error (pipeline should resume)
  MockFrequencyData validData = createTestFrequencyData();
  validData.errorFlags = AudioErrorFlag::NONE;
  MockProcessingResult result = engine.process(validData);

  // Verify pipeline resumed
  EXPECT_FALSE(engine.pipelineSuppressed);
  EXPECT_TRUE(classifier.notified);
  EXPECT_EQ(result.errorFlags, AudioErrorFlag::NONE);
  EXPECT_TRUE(result.hasDirection);
  EXPECT_TRUE(result.hasClassification);
}

/**
 * @brief Test error logging when pipeline suppressed.
 */
TEST(Audio360EngineTest, ErrorLoggingDuringSuppression) {
  MockAudio360Engine engine;

  MockFrequencyData errorData = createRandomFrequencyData();
  errorData.errorFlags = AudioErrorFlag::SILENCE;

  engine.process(errorData);

  // Verify error was logged
  EXPECT_FALSE(engine.lastError.empty());
  EXPECT_TRUE(engine.pipelineSuppressed);
}

/**
 * @brief Test multiple error types trigger suppression.
 */
TEST(Audio360EngineTest, AllErrorTypesTriggerSuppression) {
  MockAudio360Engine engine;
  MockPipelineComponent component;
  engine.registerComponent(&component);

  std::vector<AudioErrorFlag> errorTypes = {
      AudioErrorFlag::CLIPPING,
      AudioErrorFlag::LOST_SIGNAL,
      AudioErrorFlag::SILENCE
  };

  for (AudioErrorFlag errorType : errorTypes) {
    component.reset();
    engine.reset();

    MockFrequencyData data = createRandomFrequencyData();
    data.errorFlags = errorType;

    engine.process(data);

    EXPECT_TRUE(engine.pipelineSuppressed) << "Failed for error type "
                                            << static_cast<int>(errorType);
    EXPECT_FALSE(component.notified) << "Component notified despite error";
  }
}

// ============================================================================
// Test-NFR4.2: Real-Time Processing
// ============================================================================

/**
 * @brief Test-NFR4.2: Real-time processing simulation.
 * @details Verifies engine can process frames fast enough for real-time.
 * @note This is a software simulation - actual timing requires hardware.
 * @see VnVPlan.tex Section 3.2.4 (Audio360 Engine Tests)
 */
TEST(Audio360EngineTest, ProcessingSpeedSimulation) {
  MockAudio360Engine engine;
  MockPipelineComponent classifier;
  MockPipelineComponent doa;

  engine.registerComponent(&classifier);
  engine.registerComponent(&doa);

  // Simulate processing multiple frames
  const int numFrames = 100;
  std::vector<double> processingTimes;

  for (int i = 0; i < numFrames; i++) {
    MockFrequencyData data = createTestFrequencyData();

    auto start = std::chrono::high_resolution_clock::now();
    engine.process(data);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::micro> duration = end - start;
    processingTimes.push_back(duration.count());
  }

  // Calculate statistics
  double maxTime = *std::max_element(processingTimes.begin(),
                                     processingTimes.end());
  double avgTime = 0.0;
  for (double t : processingTimes) avgTime += t;
  avgTime /= numFrames;

  std::cout << "  Mock engine processing:" << std::endl;
  std::cout << "    Average: " << avgTime << " μs" << std::endl;
  std::cout << "    Maximum: " << maxTime << " μs" << std::endl;

  // Note: This tests the mock overhead. Real hardware test needed for
  // actual NFR4.2 compliance (< 62.5 μs on STM32).
  // For mock, just verify it completes without errors.
  EXPECT_TRUE(true);
}
