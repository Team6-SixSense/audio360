/**
 ******************************************************************************
 * @file    logging.hpp
 * @brief   Logging levels defintions.
 ******************************************************************************
 */
#pragma once

#ifdef STM_BUILD
#include "peripheral.h"
#endif

/* Logging macros. */
#if LOGGING_ENABLED && defined(STM_BUILD)
#define ERROR(fmt, ...) \
  printf("[ERROR] %s:%d " fmt "\n\r", __FILE__, __LINE__, ##__VA_ARGS__)
#define WARN(fmt, ...) \
  printf("[WARN] %s:%d " fmt "\n\r", __FILE__, __LINE__, ##__VA_ARGS__)
#define INFO(fmt, ...) \
  printf("[INFO]  %s:%d " fmt "\n\r", __FILE__, __LINE__, ##__VA_ARGS__)
#define DEBUG(fmt, ...) \
  printf("[DEBUG] %s:%d " fmt "\n\r", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define ERROR(fmt, ...)
#define WARN(fmt, ...)
#define INFO(fmt, ...)
#define DEBUG(fmt, ...)
#endif