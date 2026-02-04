/**
 ******************************************************************************
 * @file    filter.hpp
 * @brief   Filtering functions.
 ******************************************************************************
 */

#pragma once

#include <algorithm>
#include <deque>
#include <map>
#include <stdexcept>

/** @brief Class for filtering the mode from a list.
 *
 * Note: Type T must support operator < and operator ==.
 *
 * Note: If two values have the same frequency, the one that appears earlier
 * will be used.
 *
 */
template <typename T>
class ModeFilter {
 public:
  ModeFilter(size_t filterSize) : filterSize(filterSize) {
    if (filterSize == 0) {
      throw std::invalid_argument(
          "Mode Filter's filterSize must be greater than 0");
    }
  }

  /**
   * @brief Add a new value to the filter.
   *
   * @param newValue The new value to add.
   * @return T The value to occurs the most.
   */
  T update(T newValue) {
    // Update the history buffer.
    history.emplace_front(newValue);

    // Check if new value is most occuring.
    int newCount = ++countMap[newValue];
    if (newCount > maxCount) {
      mostOccurring = newValue;
      maxCount = newCount;
    }

    cleanBuffer();

    return mostOccurring;
  }

  /**
   * @brief Get the most occuring value in history.
   *
   * @return T The most occurinng value.
   */
  T getMostOccurring() { return mostOccurring; }

 private:
  /** @brief Clear elements beyond history buffer size. */
  void cleanBuffer() {
    bool mostOccurringDecrement{false};
    size_t numElementRemove =
        (history.size() > filterSize) ? (history.size() - filterSize) : 0;

    for (size_t i = 0; i < numElementRemove; i++) {
      T removingElement = history.back();

      if (removingElement == mostOccurring) {
        mostOccurringDecrement = true;
      }

      // Safely remove element.
      auto it = countMap.find(removingElement);
      if (it != countMap.end()) {
        if (--it->second == 0) {
          // Erase count in map to keep memory usage low.
          countMap.erase(it);
        }
      }

      history.pop_back();
    }

    if (mostOccurringDecrement) {
      scanMostOccurring();
    }
  }

  /** @brief Find the value T that occurs the most. */
  void scanMostOccurring() {
    maxCount = 0;
    for (const auto& [key, count] : countMap) {
      if (count > maxCount) {
        maxCount = count;
        mostOccurring = key;
      }
    }
  }

  /** @brief Buffer to keep track of values. Newest items are at front of queue,
   * and oldest items are at the back. */
  std::deque<T> history{};

  /** @brief The size of the @ref history buffer. */
  size_t filterSize{1};

  /** @brief map value T to number of items it appears in @ref history. */
  std::map<T, int> countMap{};

  /** @brief The most occuring value. */
  T mostOccurring{};

  /** @brief The count of the most occuring value from @ref history. */
  int maxCount = 0;
};
