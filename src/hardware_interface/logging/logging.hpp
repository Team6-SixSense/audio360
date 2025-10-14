/**
 ******************************************************************************
 * @file    logging.hpp
 * @brief   Logging levels defintions.
 ******************************************************************************
 */
#pragma once

#ifdef STM_BUILD
#include "hardware_interface/system/peripheral.h"
#endif

/* Logging macros. */
#if LOGGING_ENABLED && defined(STM_BUILD)
#define ERROR(fmt, ...) printf("[ERROR] " fmt "\n\r", ##__VA_ARGS__)
#define WARN(fmt, ...) printf("[WARN] " fmt "\n\r", ##__VA_ARGS__)
#define INFO(fmt, ...) printf("[INFO] " fmt "\n\r", ##__VA_ARGS__)
#define DEBUG(fmt, ...) printf("[DEBUG] " fmt "\n\r", ##__VA_ARGS__)
#else
#define ERROR(fmt, ...)
#define WARN(fmt, ...)
#define INFO(fmt, ...)
#define DEBUG(fmt, ...)
#endif