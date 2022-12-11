#include "KraftKontrol/utils/system_time.h"

#include "KraftKontrol/utils/topic_subscribers.h"


int64_t SystemTime::syncTimeOffset = 0;


int64_t NOW() {
    return SystemTime::getTimeSinceStart();
}


int64_t SystemTime::getTimeSinceStart() {
    return internalTime();
}


int64_t SystemTime::getSyncTime() {
    return internalTime() + syncTimeOffset;
}


int64_t SystemTime::getSystemStartDateTime() {
    return syncTimeOffset;
}


TimeDate SystemTime::getCurrentDateTime() {
    return TimeDate(getSyncTime()/SECONDS);
}


StaticCallback_Subscriber<DataTimestamped<int64_t>>& SystemTime::clockTopicSubr_() {

    static StaticCallback_Subscriber<DataTimestamped<int64_t>> clockTopicSubr(receiveTime);

    return clockTopicSubr;

}


void SystemTime::setClockSource(const Clock_Abstract& clockSource) {
    clockTopicSubr_().subscribe(clockSource.getClockTopic());
}

void SystemTime::setTime(TimeDate timeDate) {

    receiveTime(timeDate.nanosecondsTime());

}

void SystemTime::receiveTime(DataTimestamped<int64_t> const& item) {
    syncTimeOffset = item.data - item.timestamp;
}