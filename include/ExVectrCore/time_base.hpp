#ifndef EXVECTRCORE_TIMEBASE_H
#define EXVECTRCORE_TIMEBASE_H

#include "stdint.h"

namespace VCTR
{

    /**
     * @brief A function to get the system time since start in nanoseconds.
     * @note Will overflow after ~292.47 years. If a deployed system runs longer then that and crashes then give me a call.
     *       Should not be used by applications. This is to be implemented by the platform.
     *  
     * @returns time since system start in nanoseconds.
     */
    extern int64_t timeBase();

}

#endif