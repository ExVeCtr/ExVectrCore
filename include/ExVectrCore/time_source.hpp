#ifndef EXVECTRCORE_TIMESOURCE_H
#define EXVECTRCORE_TIMESOURCE_H

#include "stddef.h"
#include "stdint.h"

#include "topic_subscribers.hpp"
#include "timestamped.hpp"

#include "clock_source.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * Class for getting time from different clock sources with higher precision and performance.
         * Primarily uses internal high precision clock, but can use an external clock source to correct time for higher accuracy.
         * This corrects the internal clock by either slowing it down or speeding up.
         * @note Using an external time correction can cause jumps in time output (Not negative) or inconsistant time (Pause or slow down)
         */
        class Time_Source : public Subscriber<Timestamped<int64_t>>
        {
        private:

            float correctionLimit_ = 1.0;
            int64_t offset_ = 0;
            float factor_ = 1.0;

        public:
            /**
             * Standard constructor.
             */
            Time_Source();

            /**
             * @param source What clock to use to correct time.
             * @param correctionAmount The maximum amount to correct time. Higher values will correct faster. 2 will double of half the internal clock speed for correction. Must be larger than 1.
             */
            Time_Source(Clock_Source &source, float correctionAmount = 2.0);

            /**
             * Gets the current corrected time in nanoseconds.
             */
            int64_t NOW();

            /**
             * Gets the currect corrected time in seconds as double.
             */
            double NOWSeconds();

            /**
             * Sets the clock to be used as source.
             * @see forceCorrect() to simply correct time once
             * @param source What clock to use to correct time.
             * @param correctionAmount The maximum amount to correct time. Higher values will correct faster. 2 will double of half the internal clock speed for correction. Must be larger than 1.
             */
            void setClockSource(Clock_Source &source, float correctionAmount = 2.0);

            /**
             * Will correct time using given clock source only once. Might cause larges jumps in time.
             * @note This does not change the clock source.
             * @param source What clock to use to correct time.
             */
            void forceCorrect(Clock_Source &source);

        private:
            /**
             * It triggered by new time data published
             */
            void receive(Timestamped<int64_t> const &item, const Topic<Timestamped<int64_t>> *topic) override;
            
        };

    }

}

#endif
