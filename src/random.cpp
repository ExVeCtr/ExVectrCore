#include "ExVectrCore/random.h"

#include "math.h"

size_t VCTR::Core::randGen(size_t max, size_t seed)
{

    static size_t lastVal = 12312;
    lastVal = ((lastVal + seed) * 16807ull) % 2147483647ull;
    return lastVal % max;
}

// Generates a random number from 0 to 1.
float randGenFloat()
{
    return ((float)(VCTR::Core::randGen(UINT16_MAX)) + 1.0f) / ((float)(UINT16_MAX) + 1.0f);
}

float VCTR::Core::randNorm(float sigma, float mi)
{

    float v1 = randGenFloat();
    float v2 = randGenFloat();
    return cosf(2.0f * 3.14f * v2) * sqrtf(-2.0f * logf(v1)) * sigma + mi;
}
