#ifndef EXVECTRCORE_TIMEBOOL_H
#define EXVECTRCORE_TIMEBOOL_H

#include "stddef.h"

#include "time_definitions.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * @brief A boolean that is false after a given time is reached.
        */
        class TimedBool {
        private:

            //Time point after which the boolean is false.
            int64_t releaseTime = 0;

        public:

            /**
             * @brief Ctor for TimedBool. Value defaults to true.
             * @param releaseTime Time point after which the boolean is false.
             */
            TimedBool(int64_t releaseTime);

            /**
             * @brief Sets the value of the boolean.
             * @param value Value of the boolean.
             */
            void operator=(int64_t releaseTime);

            /**
             * @brief Gets the value of the boolean.
             * @return bool
             */
            operator bool() const;

            /**
             * @brief Gets the time point after which the boolean is false.
             * @return int64_t
             */
            int64_t getReleaseTime() const;

        };

    }

}

#endif