/**
 ******************************************************************************
 * @file    bit_operations.hpp
 * @brief   Bit operations code.
 ******************************************************************************
 */

#pragma once

#include <cstdint>

/**
 * @brief Signed extends a signed 24 bit integer value stored in an unsigned 32
 * bit integer type to a signed 32 bit integer. This essentially represents the
 * signed 24 bits integer the same in a signed 32 integer.
 *
 * @param raw The 24 bit signed integer value that will be signed extended.
 * @return int32_t signed extended represented of @ref val.
 */
inline int32_t signExtend24To32(uint32_t raw) {
  return ((int32_t)(raw << 8) >> 8);
}

/**
 * @brief Swap the order of the bytes in a 32 bit integer.
 *
 * @param raw The data to swap byte order.
 * @return int32_t representation after the byte swap of @ref raw
 */
inline uint32_t swapByteOrder32(uint32_t raw) {
  return ((raw & 0x000000FF) << 24) | ((raw & 0x0000FF00) << 8) |
         ((raw & 0x00FF0000) >> 8) | ((raw & 0xFF000000) >> 24);
}

/**
 * @brief Re-order raw microphone data. The ICS43434 stores 24-bit I2S data
 * left-aligned in the SAI data register: audio in bits [23:0], bits [31:24]
 * may contain noise from tri-state. The raw value is already a valid signed
 * int32.
 *
 * @param raw raw microphone data from the DMA buffer.
 * @return int32_t PCM data in the scale of int32.
 */
inline int32_t reorderMicData(uint32_t raw) { return (int32_t)(raw << 8); }
