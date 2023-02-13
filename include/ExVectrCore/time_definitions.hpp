#ifndef EXVECTRCORE_TIMEDEFINITIONS_H
#define EXVECTRCORE_TIMEDEFINITIONS_H

#include "stdint.h"

namespace VCTR
{

    namespace Core
    {

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

        /**
         * Gets the current internal precise clocks time in nanoseconds.
         * @note Use a Time_Source with a Clock_Source to accurate time. @see time_source.hpp
        */
        extern int64_t NOW();

        /**
         * Gets the current internal precise clocks time in seconds as double. Not as fast as NOW().
         * @note Use a Time_Source with a Clock_Source to accurate time. @see time_source.hpp
        */
        extern double NOWSeconds();

    }

}

#endif