#include "KraftKontrol/utils/value_error.h"



template<typename T = float>
extern ValueError<T> sqrt(const ValueError<T> &value) {
    return ValueError<T>(sqrt(value.value), 1.0f/(2.0f*sqrt(value.value)*value.error));
}