/**
 * @file generic_types.h
 * @brief Header file containing generic defines and typedefs.
 * @author Humza Ali
 */

#pragma once

typedef enum {
    STATUS_COMPLETE = 0,
    STATUS_ERROR    = 1,
} status_t;

#define CRITICAL_SECTION_START() noInterrupts()
#define CRITICAL_SECTION_END()   interrupts()