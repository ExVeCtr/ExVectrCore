#include "ExVectrCore/time_base.hpp"

#include "ExVectrCore/time_definitions.hpp"

namespace {} // namespace

int64_t VCTR::Core::NowNs() {
  return VCTR::Core::getPlatformClock().getCounter().data;
}

double VCTR::Core::NowS() {
  return static_cast<double>(VCTR::Core::NowNs()) /
         static_cast<double>(VCTR::Core::SECONDS);
}

void VCTR::Core::delay(int64_t delay_ns) {
  int64_t start = VCTR::Core::NowNs();
  while (VCTR::Core::NowNs() - start < delay_ns)
    ;
}