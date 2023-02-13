#include "ExVectrCore/time_source.hpp"


VCTR::Core::Time_Source::Time_Source() {}

VCTR::Core::Time_Source::Time_Source(Clock_Source &source, float correctionAmount) {
    subscribe(source.getTimeTopic());
    correctionLimit_ = correctionAmount;
}

int64_t VCTR::Core::Time_Source::NOW() {
    return NOW()*factor_ + offset_;
}

double VCTR::Core::Time_Source::NOWSeconds() {
    return static_cast<double>(NOW()) / static_cast<double>(SECONDS);
}

void VCTR::Core::Time_Source::setClockSource(Clock_Source &source, float correctionAmount) {
    subscribe(source.getTimeTopic());
    correctionLimit_ = correctionAmount;
}

void VCTR::Core::Time_Source::forceCorrect(Clock_Source &source) {
    offset_ = source.getCounter().data - source.getCounter().timestamp;
}

void VCTR::Core::Time_Source::receive(Timestamped<int64_t> const &item, const Topic<Timestamped<int64_t>> *topic) {
    
}

