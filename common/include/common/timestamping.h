#pragma once

#include <stdint.h>

#define MICROSECONDS_MULTIPLIER 1000000

typedef uint64_t MicroSecondsTimeStamp;

/**
 * @brief Returns current timestamp with microseconds precision.
 * 
 * @return MicroSecondsTimeStamp 
 */
MicroSecondsTimeStamp
getMicroSecondsTimeStamp();