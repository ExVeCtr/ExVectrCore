#ifndef EXVECTRCORE_TIMESTAMP_H
#define EXVECTRCORE_TIMESTAMP_H

#include "stdint.h"

namespace VCTR
{

    namespace Core
    {

        /**
         * Designed to take data and mark it directly with a timestamp.
         * Timestamp defaults to 0 if not set!
         */
        template <typename T>
        struct Timestamped
        {

            // Data that is timestamped
            T data;
            // Timestamp for data in nanoseconds
            int64_t timestamp;

            Timestamped()
            {
                timestamp = 0;
            }

            /**
             * @param data Sensordata to take.
             * @param timestamp Time at which sensordata was created in nanoseconds. Defaults to 0.
             */
            Timestamped(const T &data, int64_t timestamp = 0)
            {
                this->data = data;
                this->timestamp = timestamp;
            }

            /**
             * @param data Sensordata to take.
             * @param timestamp Time at which sensordata was created in nanoseconds. Defaults to 0.
             */
            Timestamped(const T &&data, int64_t timestamp = 0)
            {
                this->data = data;
                this->timestamp = timestamp;
            }
        };

    }

}

#endif