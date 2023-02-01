#ifndef EXVECTRCORE_DATATIMESTAMP_H
#define EXVECTRCORE_DATATIMESTAMP_H

#include "stdint.h"

namespace VCTR
{

    /**
     * Designed to take data and mark it directly with a timestamp.
     * Timestamp defaults to 0 if not set!
     */
    template <typename T>
    struct DataTimestamped
    {

        // Data that is timestamped
        T data;
        // Timestamp for data in nanoseconds
        int64_t timestamp;

        DataTimestamped() {
            timestamp = 0;
        }

        /**
         * @param data Sensordata to take.
         * @param timestamp Time at which sensordata was created in nanoseconds. Defaults to 0.
         */
        DataTimestamped(const T &data, int64_t timestamp = 0)
        {
            this->data = data;
            this->timestamp = timestamp;
        }

        /**
         * @param data Sensordata to take.
         * @param timestamp Time at which sensordata was created in nanoseconds. Defaults to 0.
         */
        DataTimestamped(const T &&data, int64_t timestamp = 0)
        {
            this->data = data;
            this->timestamp = timestamp;
        }
    };

}

#endif