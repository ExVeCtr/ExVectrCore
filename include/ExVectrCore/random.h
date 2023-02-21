#ifndef EXVECTRCORE_RANDOM_H
#define EXVECTRCORE_RANDOM_H

#include "stdint.h"

namespace VCTR
{

    namespace Core
    {

        /**
         * @brief Generates a random 16bit unsigned integer up to the given max (excluding max and including 0). Giving a seed will reset the generator using the given seed.
         *
         * @param max Maximum number to generate (Exclusive).
         * @param seed Starting seed and resetting algorithm if other than 0.
         * @return uint16_t
         */
        extern uint16_t randGen(uint16_t max = UINT16_MAX, uint32_t seed = 0);

        /**
         * @brief Generates a normally distributed random number.
         *
         * @param sigma How spread the values are. (Variance)
         * @param mi Where the spread is. (Average)
         * @return float
         */
        extern float randNorm(float sigma = 1, float mi = 0);

    }
}

#endif