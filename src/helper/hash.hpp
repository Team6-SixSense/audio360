/**
******************************************************************************
* @file    hash.hpp
* @brief   Contains hashing functions.
******************************************************************************
*/

#pragma once

#include <cstdint>

const static uint32_t HASH_START = 2166136261u;
const static uint32_t HASH_MULTIPLIER = 2166136261u;

/**
 * @brief Hashing function for a stream of data.
 *
 * @param data Streamed data.
 * @param n Number of elements in the stream
 * @return uint32_t 32 bit hash of the data stream.
 */
static uint32_t fnv1a_hash32(const int32_t* data, size_t n) {
  uint32_t hash = HASH_START;
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
  size_t len = n * sizeof(int32_t);

  for (size_t i = 0; i < len; ++i) {
    hash ^= bytes[i];
    hash *= HASH_MULTIPLIER;
  }

  return hash;
}
