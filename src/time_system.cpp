
#include "ExVectrCore/time_system.hpp"

#include "ExVectrHAL/time_hal.hpp"


namespace 
{
    //Offset used to correct accurate clock
    int64_t syncTimeOffset = 0;

    //Time factor used to speed up or slow down time for testing and simulating.
    float timeFactor = 1;

} // namespace 



double VCTR::NOWSeconds() {
    return static_cast<double>(NOW()) / SECONDS;
}


int64_t VCTR::NOW() {
    return internalTime() * timeFactor;
}


int64_t VCTR::SystemTime::getSyncTime() {
    return NOW() + syncTimeOffset;
}


int64_t VCTR::SystemTime::getSystemStartDateTime() {
    return syncTimeOffset;
}


VCTR::TimeDate VCTR::SystemTime::getCurrentDateTime() {
    return VCTR::TimeDate(getSyncTime() / SECONDS);
}

void VCTR::SystemTime::updateAccurateClock(DataTimestamped<int64_t> const& item) {
    syncTimeOffset = item.data - item.timestamp;
}