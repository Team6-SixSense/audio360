/**
 ******************************************************************************
 * @file    mode_filter_test.cpp
 * @brief   Unit tests for mode filtering.
 ******************************************************************************
 */

#include "mode_filter_test.hpp"

#include <vector>

#include "filter.hpp"

/* ============================== INTEGER TESTS ============================= */

/** @brief Assert that mode filtering throws an invalid argument exception when
 * filter size is 0. */
TEST(ModeFilterIntTest, InvalidArgs) {
  EXPECT_THROW(ModeFilter<int>(0), std::invalid_argument);
}

/** @brief Verifiy that updating the buffer will return the correct mode. */
TEST_P(ModeFilterIntTest, CorrectModeMultiUpdate) {
  const auto& param = this->GetParam();

  // Assert correct test parameter sizes.
  ASSERT_EQ(param.input.size(), param.numUpdates);
  ASSERT_EQ(param.expectedMostOccurring.size(), param.numUpdates);

  // Instantiate the filter.
  ModeFilter<int> filter(param.filterSize);

  // Update the filter and assert the expected mode is correct.
  for (size_t i = 0; i < param.numUpdates; i++) {
    int mode = filter.update(param.input[i]);
    ASSERT_EQ(mode, param.expectedMostOccurring[i]);
    ASSERT_EQ(filter.getMostOccurring(), param.expectedMostOccurring[i]);
  }
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(IntModeFilterValues, ModeFilterIntTest,
                         ::testing::ValuesIn(GetIntTestConfigs()));

/* ================================ ENUM TESTS ============================== */

/** @brief Assert that mode filtering throws an invalid argument exception when
 * filter size is 0. */
TEST(ModeFilterEnumTest, InvalidArgs) {
  EXPECT_THROW(ModeFilter<ClassificationLabel>(0), std::invalid_argument);
}

/** @brief Verifiy that updating the buffer will return the correct mode. */
TEST_P(ModeFilterEnumTest, CorrectModeMultiUpdate) {
  const auto& param = this->GetParam();

  // Assert correct test parameter sizes.
  ASSERT_EQ(param.input.size(), param.numUpdates);
  ASSERT_EQ(param.expectedMostOccurring.size(), param.numUpdates);

  // Instantiate the filter.
  ModeFilter<ClassificationLabel> filter(param.filterSize);

  // Update the filter and assert the expected mode is correct.
  for (size_t i = 0; i < param.numUpdates; i++) {
    ClassificationLabel mode = filter.update(param.input[i]);
    ASSERT_EQ(mode, param.expectedMostOccurring[i]);
    ASSERT_EQ(filter.getMostOccurring(), param.expectedMostOccurring[i]);
  }
}

/** @brief Parametized options. */
INSTANTIATE_TEST_SUITE_P(EnumModeFilterValues, ModeFilterEnumTest,
                         ::testing::ValuesIn(GetEnumTestConfigs()));
