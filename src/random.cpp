#include "ExVectrCore/random.h"


#include "math.h"


uint16_t VCTR::Core::randGen(uint16_t max = UINT16_MAX, uint32_t seed = 0) {

    static uint32_t lastVal = 12312;
    if (seed != 0) lastVal = seed % (123124 * 5 / 7);

    lastVal = (lastVal * 1231 - 5123) % 123124;

    return lastVal % max;

}

//Generates a random number from 0 to 1.
float randGenFloat() {
    return ( (float)(VCTR::Core::randGen()) + 1. )/( (float)(UINT16_MAX) + 1. );
}


float VCTR::Core::randNorm(float sigma = 1, float mi = 0) {

    float v1=randGenFloat();
    float v2=randGenFloat();
    return cos(2.0f*3.14f*v2)*sqrt(-2.0f*log(v1)) * sigma + mi;

}


