#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H



#include "Arduino.h"



template<typename T>
class LowPassFilter {
public:


    /**
     * Constructor for no filtering.
     * Cutoff = 1Hz
     *
     * @param values none
     * @return none.
     */
    LowPassFilter() {

        _sampleRate = -1;
        _cutOffFreq = 1;
        _RC = 1.0/(_cutOffFreq*2*3.14);

        if (_sampleRate > 0) {
            float dt = 1.0/_sampleRate;
            _alpha = dt/(_RC+dt);
        }

    }


    /**
     * Give sampleRate to improve performance
     *
     * @param values cutOffFreq and sampleRate
     * @return none.
     */
    LowPassFilter(float cutOffFreq, int32_t sampleRate = -1) {

        _sampleRate = sampleRate;
        _cutOffFreq = cutOffFreq;
        _RC = 1.0/(_cutOffFreq*2*3.14);

        if (_sampleRate > 0) {
            float dt = 1.0/_sampleRate;
            _alpha = dt/(_RC+dt);
        }

    }


    /**
     *
     * @param values input value
     * @return filtered value.
     */
    T update(T input) {
        
        if (_sampleRate < 0) {
            float dt = (double)(NOW() - _lastRun)/SECONDS;
            _lastRun = NOW();
            _alpha = dt/(_RC+dt);
        }

        T output = _lastValue + (input - _lastValue)*_alpha;
        _lastValue = output;

        return output;

    }


    /**
     * Timestamp of input value (in microseconds) to improve accuracy.
     * This overrides the sampling input at constructor.
     *
     * @param values input value and timestamp in nanoseconds
     * @return filtered value.
     */
    T update(const T &input, int64_t timestamp) {
        
        float dt = (double)(timestamp - _lastRun)/SECONDS;
        _lastRun = timestamp;
        _alpha = dt/(_RC+dt);

        T output = _lastValue + (input - _lastValue)*_alpha;
        _lastValue = output;

        return output;

    }


    /**
     * Ccanges the response frequency
     * @param freq
     * @param sampleRate
     */
    void setParameters(float freq, float sampleRate = -1) {

        _cutOffFreq = freq;
        _sampleRate = sampleRate;
        
        _RC = 1.0/(_cutOffFreq*2*3.14);

        if (_sampleRate > 0) {
            float dt = 1.0/_sampleRate;
            _alpha = dt/(_RC+dt);
        }

    }


    /**
     * Returns that current filtered value.
     *
     * @param values none
     * @return filtered value.
     */
    T getValue() {return _lastValue;}


    /**
     * Sets the value to given parameter
     *
     * @param values input value
     * @return none.
     */
    void setValue(const T &input) {
        
        _lastValue = input;

    }



private:

    int32_t _sampleRate;
    float _cutOffFreq;
    float _RC;

    float _alpha;

    T _lastValue;
    int64_t _lastRun = 0;


};



#endif