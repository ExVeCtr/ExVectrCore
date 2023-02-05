#ifndef EXVECTRCORE_SCHEDULER_H
#define EXVECTRCORE_SCHEDULER_H

#include "stddef.h"
#include "string.h"
#include "stdint.h"

#include "time_system.hpp"

namespace VCTR
{

    /**
     * New Scheduler design features:
     * - Measure task runtime
     * - Automatic schedule planning
     * - No priorities, rather realtime importance.
     * - Same timing structure as old system.
     * - Supports event triggers (Based off of events). This allows cascading tasks (Run if another has ran).
     * - Timing relative to precise or accurate clock.
     * - Support for sleep.
     * - Multiple schedulers for multi-core. (Maybe this allows for support of precise or accurate clock?)
     * 
     * - Maybe AI search for optimal task planning?
     * 
    */

}

#endif
