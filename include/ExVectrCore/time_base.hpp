#ifndef EXVECTRCORE_TIMEBASE_H
#define EXVECTRCORE_TIMEBASE_H

#include "stdint.h"

#include "clock_source.hpp"

namespace VCTR
{

    namespace Core
    {
        
        /**
         * To be implemented by the platform.
         * @note Must be very fast.
         * @returns the precise clock from the platform. 
        */
        extern const Clock_Source& getPlatformClock();

    }

}

#endif