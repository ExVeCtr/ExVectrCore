#include "ExVectrCore/time_definitions.hpp"

#include "ExVectrCore/time_source.hpp"

VCTR::Core::Time_Source::Time_Source() {}

VCTR::Core::Time_Source::Time_Source(Clock_Source &source,
                                     float correctionAmount) {
  subscribe(source.getTimeTopic());
  correctionLimit_ = correctionAmount;
}

int64_t VCTR::Core::Time_Source::Now() {
  return int64_t(VCTR::Core::NowNs() * factor_) + offset_;
}

double VCTR::Core::Time_Source::NOWSeconds() {
  return static_cast<double>(this->Now()) /
         static_cast<double>(VCTR::Core::SECONDS);
}

void VCTR::Core::Time_Source::setClockSource(Clock_Source &source,
                                             float correctionAmount) {
  subscribe(source.getTimeTopic());
  correctionLimit_ = correctionAmount;
}

void VCTR::Core::Time_Source::forceCorrect(Clock_Source &source) {
  offset_ = source.getCounter().data - source.getCounter().timestamp;
}

void VCTR::Core::Time_Source::setTime(int64_t time) {
  offset_ = time - VCTR::Core::NowNs();
}

void VCTR::Core::Time_Source::receive(
    Timestamped<int64_t> const &item,
    const Topic<Timestamped<int64_t>> *topic) {
  setTime(item.data + (Core::NowNs() - item.timestamp));
}
