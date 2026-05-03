#ifndef EXVECTRCORE_TIMEDEFINITIONS_H
#define EXVECTRCORE_TIMEDEFINITIONS_H

#include "stdint.h"

namespace VCTR {

namespace Core {

constexpr int64_t NANOSECONDS = 1;
constexpr int64_t MICROSECONDS = 1000 * NANOSECONDS;
constexpr int64_t MILLISECONDS = 1000 * MICROSECONDS;
constexpr int64_t SECONDS = 1000 * MILLISECONDS;
constexpr int64_t MINUTES = 60 * SECONDS;
constexpr int64_t HOURS = 60 * MINUTES;
constexpr int64_t DAYS = 24 * HOURS;
constexpr int64_t WEEKS = 7 * DAYS;
constexpr int64_t MONTHS = 30 * DAYS;
constexpr int64_t YEARS = 365 * DAYS;

/// @brief The end of time for precise clock.
constexpr int64_t END_OF_TIME = INT64_MAX;

// The ESP8266 SDK (eagle_soc.h) defines NOW() as a macro that reads a HW timer
// register. We must undefine it so our function declaration compiles. This is
// safe: the SDK only uses NOW() in pre-compiled .a libraries, not in any
// headers.
#ifdef NOW
#undef NOW
#endif

/**
 * Gets the current internal precise clocks time in nanoseconds.
 * @note Use a Time_Source with a Clock_Source to accurate time. @see
 * time_source.hpp
 */
extern int64_t NOW();

/**
 * Gets the current internal precise clocks time in seconds as double. Not as
 * fast as NOW().
 * @note Use a Time_Source with a Clock_Source to accurate time. @see
 * time_source.hpp
 */
extern double NOWSeconds();

/**
 * @brief Blocks everything for given amount of time in nanoseconds.
 * @note DO NOT USE. WASTES CPU TIME AND BLOCKS ALL OTHER THINGS THAT NEED THE
 * TIME! Only included to support older code. Will be removed in the future!
 * @note Possible that in the future this will yield the given time to other
 * threads. Will probably be removed though.
 * @param delay_ns Time in nanoseconds.
 */
extern void delay(int64_t delay_ns);

} // namespace Core

} // namespace VCTR

#endif