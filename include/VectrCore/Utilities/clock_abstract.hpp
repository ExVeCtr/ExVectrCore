#ifndef VECTRCORE_UTILITIES_CLOCKABSTRACT_H
#define VECTRCORE_UTILITIES_CLOCKABSTRACT_H

#include "data_timestamped.hpp"

#include "topic.hpp"

namespace VCTR
{

    class Clock_Abstract
    {
    private:
    protected:
        Topic<DataTimestamped<int64_t>> clockTopic_;

    public:
        const Topic<DataTimestamped<int64_t>> &getClockTopic() const { return clockTopic_; }
    };

}

#endif