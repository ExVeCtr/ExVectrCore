#include "ExVectrCore/time_definitions.hpp"

#include "ExVectrCore/time_base.hpp"


namespace 
{
    

} // namespace to hide local variables.



int64_t VCTR::Core::NOW() {
    return VCTR::Core::getPlatformClock().getCounter().data;
}


double VCTR::Core::NOWSeconds() {
    return static_cast<double>(VCTR::Core::NOW()) / static_cast<double>(VCTR::Core::SECONDS);
}

void VCTR::Core::delay(int64_t delay_ns) {
    int64_t start = VCTR::Core::NOW();
    while(VCTR::Core::NOW() - start < delay_ns);
}