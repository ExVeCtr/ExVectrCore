#ifndef EXVECTRCORE_CLOCKSOURCE_H
#define EXVECTRCORE_CLOCKSOURCE_H

#include "stddef.h"
#include "stdint.h"

#include "topic.hpp"
#include "timestamped.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * Interface class for a clock source. Offers a topic to publish new time information.
         * @note getCounter() can be expensive depending on the clock source e.g. if external clock over I2C connection.
         * @note Please use Time_Source for application as it offers higher performance.
         */
        class Clock_Source
        {
        protected:
            Topic<Timestamped<int64_t>> timeTopic_;

        public:
            /**
             * Gets the current clocks time in nanoseconds timestamped with internal precise. Might be slow if using an external clock source.
             */
            virtual const Timestamped<int64_t>& getCounter() const = 0;

            /**
             * Gets the topic to use for receiving new time information.
             */
            Topic<Timestamped<int64_t>> &getTimeTopic();
        };

    }

}

#endif
