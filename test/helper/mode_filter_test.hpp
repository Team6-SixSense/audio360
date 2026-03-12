/**
 ******************************************************************************
 * @file    mode_filter_test.hpp
 * @brief   Header file with some source code for mode filtering unit tests.
 ******************************************************************************
 */

#pragma once

#include <gtest/gtest.h>

#include "classificationLabel.h"

/** @brief Struct for parameterized testing. */
template <typename T>
struct ModeFilterParamType {
  size_t filterSize;  // Filter size.

  size_t numUpdates;  // Number of update function calls.

  std::vector<T> input;  // Value to insert in each update call in order.

  std::vector<T> expectedMostOccurring;  // Expected most occurring return value
                                         // for each update call.
};

/** @brief Parameterized test class for update function of mode filtering of
 * integer type. */
class ModeFilterIntTest
    : public ::testing::TestWithParam<ModeFilterParamType<int>> {};

/** @brief Parameterized test class for update function of mode filtering of
 * classifican label enum type. */
class ModeFilterEnumTest : public ::testing::TestWithParam<
                               ModeFilterParamType<ClassificationLabel>> {};

/**
 * @brief Create parameterized test configuration for mode filter integer tests.
 *
 * @return std::vector<ModeFilterParamType<int>> Parameterized test
 * configurations.
 */
std::vector<ModeFilterParamType<int>> GetIntTestConfigs() {
  // Parameterized test 1. Tests that given the same input value, the mode
  // remains that value.
  std::vector<int> allOnes(10, 1);

  // Parameterized test 2. Tests that mode changes correctly with different
  // values.
  std::vector<int> modeChangesInput{1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2};
  std::vector<int> modeChangesExpected{1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2};

  // Parameterized test 3. Similar to test 2 but tests when size of internal
  // buffer is even.
  std::vector<int> equalFreqInput{1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2};
  std::vector<int> equalFreqExpected{1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};

  // Parameterized test 4. Tests that mode detection of internal buffer size 1.
  std::vector<int> sizeOneResults{1, 2, 3, 4, 5, 6};

  return {
      {5, 10, allOnes, allOnes},                       // Test 1.
      {5, 12, modeChangesInput, modeChangesExpected},  // Test 2.
      {6, 12, equalFreqInput, equalFreqExpected},      // Test 3.
      {1, 6, sizeOneResults, sizeOneResults},          // Test 4.
      {0, 10, allOnes, allOnes}                        // Filter size 0 test.
  };
}

/**
 * @brief Create parameterized test configuration for mode filter classfication
 * label enum tests.
 *
 * @return std::vector<ModeFilterParamType<ClassificationLabel>> Parameterized
 * test configurations.
 */
std::vector<ModeFilterParamType<ClassificationLabel>> GetEnumTestConfigs() {
  // Parameterized test 1. Tests that given the same input value, the mode
  // remains that value.
  std::vector<ClassificationLabel> allSirens(10, ClassificationLabel::Siren);

  // Parameterized test 2. Tests that mode changes correctly with different
  // values.
  std::vector<ClassificationLabel> modeChangesInput{
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn};
  std::vector<ClassificationLabel> modeChangesExpected{
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn};

  // Parameterized test 3. Similar to test 2 but tests when size of internal
  // buffer is even.
  std::vector<ClassificationLabel> equalFreqInput{
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn};
  std::vector<ClassificationLabel> equalFreqExpected{
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::Siren,   ClassificationLabel::Siren,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn,
      ClassificationLabel::CarHorn, ClassificationLabel::CarHorn};

  // Parameterized test 4. Tests that mode detection of internal buffer size 1.
  std::vector<ClassificationLabel> sizeOneResults{
      ClassificationLabel::Unknown, ClassificationLabel::Siren,
      ClassificationLabel::Jackhammer, ClassificationLabel::CarHorn,
      ClassificationLabel::Clapping};

  return {
      {5, 10, allSirens, allSirens},                   // Test 1.
      {5, 12, modeChangesInput, modeChangesExpected},  // Test 2.
      {6, 12, equalFreqInput, equalFreqExpected},      // Test 3.
      {1, 5, sizeOneResults, sizeOneResults},          // Test 4.
      {0, 10, allSirens, allSirens}                    // Filter size 0 test.
  };
}
