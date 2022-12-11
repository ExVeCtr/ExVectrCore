#ifndef VECTRCORE_UTILITIES_SYSTEMTIME_H
#define VECTRCORE_UTILITIES_SYSTEMTIME_H

#include "time_data.hpp"
#include "time_definitions.hpp"

#include "data_timestamped.hpp"

#include "stdint.h"

namespace VCTR
{

    namespace SystemTime
    {

        /**
         * @brief Get the local time in nanoseconds since system start. (Precise clock)
         *
         * @return int64_t
         */
        extern int64_t getTimeSinceStart();

        /**
         * @brief Get the time synchronised to be accurate (Accurate clock). Usually a real time clock set to Unix time is used to correct this time.
         *
         * @return int64_t
         */
        extern int64_t getSyncTime();

        /**
         * @brief Get the TimeDate of when the system started.
         *
         * @return int64_t
         */
        extern int64_t getSystemStartDateTime();

        /**
         * @brief Get the Current TimeDate. (Accurate clock)
         *
         * @return DateTime
         */
        extern TimeDate getCurrentDateTime();

        /**
         * @brief Updates the accurate clock with the given time measurement at given system time. Will cause jumps in the accurate time if correction is made.
        */
        extern void updateAccurateClock(DataTimestamped<int64_t> const &item);

    }; ///The system time namespace contains functions for the accurate and precise clocks

}

#endif