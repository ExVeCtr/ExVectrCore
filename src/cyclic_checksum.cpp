#include "stddef.h"
#include "stdint.h"

#include "ExVectrCore/cyclic_checksum.hpp"

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
        uint32_t computeCrc(const uint8_t* buf, int32_t len, int32_t initialValue) {

            int32_t currentValue = initialValue;

            for(int charCnt = 0; charCnt < len; charCnt++) {
                char curChar = buf[charCnt];
                for(int bitCnt = 0; bitCnt < 8; bitCnt++) {
                    if((curChar & 0x80) ^ ((currentValue & 0x8000) >> 8)) {
                        currentValue = ((currentValue << 1)  ^ 0x1021) & 0xFFFF; // Standard Polynom for CCSDS
                    } else {
                        currentValue = (currentValue << 1) & 0xFFFF;
                    }
                    curChar = curChar << 1;
                }
            }
            return currentValue;

        }

    }

}
