/**
 * @file generic_types.h
 * @brief Header file containing generic defines and typedefs.
 * @author Humza Ali
 */

#pragma once

/**
 * @enum status_t
 * @brief Status codes used to indicate the result of a task.
 */
typedef enum {
    /** Indicates a failure in initializing an IMU. */
    STATUS_IMU_INIT_FAILURE = -1,
    /** Indicates completion of a task. */
    STATUS_COMPLETE         = 0,
    /** Indicates a NULL pointer that should've been set. */
    STATUS_NULL_POINTER     = 1,
} status_t;

// Set to 1 to print logs through Serial output
#define SERIAL_OUTPUT_LOGGING 0
#if SERIAL_OUTPUT_LOGGING
// Set to 1 to print debug data
#define DEBUG 0
#endif

/** Typedef used for representing GPIO pin numbers.  */
typedef uint8_t Pins_t;