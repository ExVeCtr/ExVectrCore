#ifndef EXVECTRCORE_CYCLIC_CHECKSUM_HPP
#define EXVECTRCORE_CYCLIC_CHECKSUM_HPP

#include "stddef.h"
#include "stdint.h"

namespace VCTR
{

    namespace Core
    {
            
        /**
         * Computes the CRC-16-CCITT checksum for the given buffer.
         * @param buf The buffer to compute the checksum for.
         * @param len The length of the buffer.
         * @param initialValue The initial value for the checksum.
         * @return The computed checksum.
         */
        uint32_t computeCrc(const uint8_t* buf, int32_t len, int32_t initialValue);

    }

}

#endif
