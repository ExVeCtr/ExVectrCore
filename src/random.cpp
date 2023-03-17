#include "ExVectrCore/random.h"

#include "math.h"
#include "stdint.h"

size_t VCTR::Core::randGen(size_t max, size_t seed)
{

    static size_t lastVal = 12312;
    lastVal = ((lastVal + seed) * 16807ull) % 2147483647ull;
    return lastVal % max;
}

// Generates a random number from 0 to 1.
float randGenFloat()
{   
    //Note: (uint16_t)-1 is the maximum size of a 16 bit unsigned integer. This is the more portable methode of the constant UINT16_MAX
    return ((float)(VCTR::Core::randGen((uint16_t)-1)) + 1.0f) / ((float)((uint16_t)-1) + 1.0f);
}

float VCTR::Core::randNorm(float sigma, float mi)
{

    float v1 = randGenFloat();
    float v2 = randGenFloat();
    return cosf(2.0f * 3.14f * v2) * sqrtf(-2.0f * logf(v1)) * sigma + mi;
}
