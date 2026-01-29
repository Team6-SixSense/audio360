/**
******************************************************************************
 * @file    utils.h
 * @brief   Contains any utility helper functions used by runtime_audio360.cpp
 ******************************************************************************
 */

#include <cstdint>

static std::uint32_t fnv1a_hash32(const int32_t* data, std::size_t n) {
  uint32_t h = 2166136261u;
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
  std::size_t len = n * sizeof(int32_t);
  for (std::size_t i = 0; i < len; ++i) {
    h ^= bytes[i];
    h *= 16777619u;
  }
  return h;
}