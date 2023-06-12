#ifndef EXVECTRCORE_H_
#define EXVECTRCORE_H_

#include "ExVectrCore/scheduler2.hpp"
#include "ExVectrCore/print.hpp"
#include "ExVectrCore/time_definitions.hpp"
#include "ExVectrCore/topic.hpp"
#include "ExVectrCore/topic_subscribers.hpp"

namespace VCTR
{
    namespace Core
    {
        
        /**
         * Initialises all internals of the platform implementation. 
         * For example console output linking in the print.hpp header.
         * @note Must be called as the very first thing when the application starts.
        */
        extern void initialise();

    } // namespace Core
} // namespace VCTR The EXVECTR Library

#endif