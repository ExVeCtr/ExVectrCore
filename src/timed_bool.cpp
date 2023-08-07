#include "stddef.h"

#include "ExVectrCore/time_definitions.hpp"
#include "ExVectrCore/timed_bool.hpp"

namespace VCTR
{

    namespace Core
    {

        TimedBool::TimedBool(int64_t releaseTime) : releaseTime(releaseTime) {}

        void TimedBool::operator=(int64_t releaseTime) {
            this->releaseTime = releaseTime;
        }   

        TimedBool::operator bool() const {
            return NOW() < releaseTime;
        }

        int64_t TimedBool::getReleaseTime() const {
            return releaseTime;
        }

    }

}