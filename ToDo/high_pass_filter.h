#ifndef HIGH_PASS_FILTER_H
#define HIGH_PASS_FILTER_H



#include "Arduino.h"



template<typename T>
class HighPassFilter {
public:


    /**
     * Give sampleRate to improve performance
     *
     * @param values cutOffFreq and sampleRate
     * @return none.
     */
    HighPassFilter(float cutOffFreq, uint32_t sampleRate = 0) {

        _sampleRate = sampleRate;
        _cutOffFreq = cutOffFreq;
        _RC = 1.0/(cutOffFreq*2*3.14);

        if (_sampleRate != 0) {
            float dt = 1.0/_sampleRate;
            _alpha = _RC/(_RC+dt);
        }

    }


    /**
     *
     * @param values input value
     * @return filtered value.
     */
    T update(T input) {
        
        if (_sampleRate == -1) {
            float dt = (micros() - _lastRun)/1000000.0;
            _lastRun = micros();
            _alpha = _RC/(_RC+dt);
        }

        T output = (_lastOutputValue + input - _lastInputValue)*_alpha;
        _lastInputValue = input;
        _lastOutputValue = output;

        return output;

    }


    /**
     * Timestamp of input value (in microseconds) to improve accuracy.
     * This overrides the sampling input at constructor.
     *
     * @param values input value and timestamp in microseconds
     * @return filtered value.
     */
    T update(T input, uint32_t timestampUS) {
        
        float dt = (timestampUS - _lastRun)/1000000.0;
        _lastRun = timestampUS;
        _alpha = _RC/(_RC+dt);

        T output = (_lastOutputValue + input - _lastInputValue)*_alpha;
        _lastInputValue = input;
        _lastOutputValue = output;

        return output;

    }

    /**
     * Sets the value to given parameter
     *
     * @param values input value
     * @return none.
     */
    void setValue(const T &input) {
        
        _lastOutputValue = _lastInputValue = input;

    }


private:

    uint32_t _sampleRate;
    float _cutOffFreq;
    float _RC;

    float _alpha;

    T _lastOutputValue;
    T _lastInputValue;
    uint32_t _lastRun = 0;


};



#endif