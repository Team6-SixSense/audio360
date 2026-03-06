/**
 ******************************************************************************
 * @file    exceptions.hpp
 * @brief   exceptions
 ******************************************************************************
 */

#pragma once

#include <exception>
#include <string>

class AudioProcessingException : public std::exception {
 public:
  /**
   * @brief Construct an audio processing exception.
   *
   * @param msg Exception message.
   */
  AudioProcessingException(const char* msg)
      : message(std::string("AudioProcessingException: ") + msg) {}

  /**
   * @brief Overrides what() method.
   *
   * @return const char* Message string.
   */
  const char* what() const noexcept { return message.c_str(); }

 private:
  /** @brief Exception message. */
  std::string message;
};
