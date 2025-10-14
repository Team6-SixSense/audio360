/**
 ******************************************************************************
 * @file    logging.hpp
 * @brief   Logging levels defintions.
 ******************************************************************************
 */

#include "hardware_interface/system/peripheral.h"

/* Logging macros. */
#if LOGGING_ENABLED
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