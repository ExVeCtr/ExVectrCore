#include "ExVectrCore/clock_source.hpp"

#include "stddef.h"
#include "stdint.h"

#include "ExVectrCore/topic.hpp"
#include "ExVectrCore/timestamped.hpp"


VCTR::Core::Topic<VCTR::Core::Timestamped<int64_t>> & VCTR::Core::Clock_Source::getTimeTopic() {
    return timeTopic_;
}

