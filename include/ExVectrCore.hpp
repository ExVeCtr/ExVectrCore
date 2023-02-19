#ifndef EXVECTRCORE_H_
#define EXVECTRCORE_H_

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