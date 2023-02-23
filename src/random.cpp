#include "ExVectrCore/random.h"


#include "math.h"


size_t VCTR::Core::randGen(size_t max, uint32_t seed) {

    static uint32_t lastVal = 12312;
    if (seed != 0) lastVal = seed % (123124 * 5 / 7);

    lastVal = (lastVal * 1231 - 5123) % 123124;

    return lastVal % max;

}

//Generates a random number from 0 to 1.
float randGenFloat() {
    return ( (float)(VCTR::Core::randGen(UINT16_MAX)) + 1.0f )/( (float)(UINT16_MAX) + 1.0f );
}


float VCTR::Core::randNorm(float sigma, float mi) {

    float v1=randGenFloat();
    float v2=randGenFloat();
    return cosf(2.0f*3.14f*v2)*sqrtf(-2.0f*logf(v1)) * sigma + mi;

}


