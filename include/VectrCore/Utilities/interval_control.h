#ifndef VECTRCORE_UTILITIES_INTERVALCONTROL_H
#define VECTRCORE_UTILITIES_INTERVALCONTROL_H

#include "system_time.hpp"

namespace VCTR
{

    class IntervalControl
    {
    private:
        int64_t lastRun_ns_ = 0;

        int64_t interval_ns_ = 0;

        bool limit_ = false;

        bool block_ = false;

    public:
        IntervalControl()
        {
            lastRun_ns_ = 0;
        }

        IntervalControl(float rate)
        {
            lastRun_ns_ = 0;
            if (rate > 0.0f)
                setRate(rate);
        }

        /**
         * Sets the rate in Hz
         *
         * @param values rate in Hz
         */
        inline void setRate(float rate_Hz) { interval_ns_ = (int64_t)SECONDS / rate_Hz; }

        /**
         * Gets the rate in Hz
         *.
         * @return uint32_t.
         */
        inline uint32_t getRate() { return SECONDS / interval_ns_; }

        /**
         * Sets the interval in milliseconds
         *
         * @param values interval in milliseconds
         */
        inline void setInterval(int64_t interval_ns)
        {
            interval_ns_ = interval_ns;
            lastRun_ns_ = NOW() - interval_ns_;
        }

        /**
         * Gets the interval in milliseconds
         *.
         * @return interval in milliseconds.
         */
        inline int64_t getInterval() { return interval_ns_; }

        /**
         * Returns the amount of time till the next run.
         * Negative number means the how long ago it should have ran
         *
         * @return remaining time till next run.
         */
        inline int64_t getTimeRemain() { return -interval_ns_ + NOW() + lastRun_ns_; }

        /**
         * Returns the amount of time till the next run.
         * Negative number means the how long ago it should have ran
         *
         * @return remaining time till next run.
         */
        inline int64_t getTimeToRun() { return interval_ns_ + lastRun_ns_; }

        /**
         * Sets the way the system limits the runs.
         * If limit is true then this will only limit the rate
         * and if the run takes longer than the set interval the
         * system will not compensate for the lost time.
         * If limit is false then if a run is late the system will make
         * the next runs come sooner. This causes abnormal intervals but
         * is usefull if somthing has to run a certain amount of times
         * per second.
         *
         * @param limit true to limit
         */
        inline void setLimit(bool limit) { limit_ = limit; }

        /**
         * Sets internal clock so that the next call to whether or not to run will always return true.
         */
        inline void forceNextRun() { lastRun_ns_ = NOW() - interval_ns_; }

        /**
         * Resets internal clock so that the next run will be in the set interval from now.
         */
        inline void syncClock() { lastRun_ns_ = NOW(); }

        /**
         * Waits till next Run.
         * See overload to use this time for other functions
         *
         */
        inline void waitTillNextRun()
        {

            while (NOW() - lastRun_ns_ < interval_ns_ || block_)
                ;

            if (limit_)
                lastRun_ns_ = NOW();
            else
                lastRun_ns_ += interval_ns_;
        }

        /**
         * Waits till next Run but calls the given
         * method in the time it waits
         *
         * @param callMethod method to run while waiting
         */
        inline void waitTillNextRun(void (*callMethod)(void))
        {

            while (NOW() - lastRun_ns_ < interval_ns_ || block_)
                callMethod();

            if (limit_)
                lastRun_ns_ = NOW();
            else
                lastRun_ns_ = NOW() - (NOW() % interval_ns_);
        }

        /**
         * Checks if it is time to run and returns
         * true if its time to run.
         * If updateClock is true then the internal interval
         * is set for next run. If you only want to check if its
         * time to run then set updateClock to false;
         * Usefull for if statements.
         * e.g:
         *
         * if (x.isTimeToRun()) {
         *      Stuff to do periodically...
         * }
         *
         * @param updateClock Set to true to update internal clock for next run. Defaults to true.
         * @returns true if it is time to run
         */
        inline bool isTimeToRun(bool updateClock = true)
        {

            if (block_)
                return false;

            if (NOW() - lastRun_ns_ > interval_ns_)
            {
                if (!limit_ && updateClock)
                    lastRun_ns_ = NOW() - (NOW() % interval_ns_); //{while(lastRun_ns_ + interval_ns_ < NOW()) lastRun_ns_ += interval_ns_;}
                else if (updateClock)
                    lastRun_ns_ = NOW();
                return true;
            }
            else
                return false;
        }

        /**
         * Checks if it is time to run and returns
         * true if its time to run.
         * If updateClock is true then the internal interval
         * is set for next run. If you only want to check if its
         * run to run then set updateClock to false;
         * Usefull for if statements.
         * This overload will write the time delta from the
         * last run into the timeDelta variable. This way
         * the time from the last run is passed on.
         * e.g:
         *
         * if (x.isTimeToRun()) {
         *      Stuff to do periodically...
         * }
         *
         * @param timeDelta time since last run.
         * @param updateClock Set to true to update internal clock for next run. Defaults to true.
         * @returns true if it is time to run
         */
        inline bool isTimeToRun(int64_t &timeDelta, bool updateClock = true)
        {

            if (block_)
                return false;

            timeDelta = NOW() - lastRun_ns_;
            if (timeDelta > interval_ns_)
            {
                if (!limit_ && updateClock)
                    lastRun_ns_ = NOW() - (NOW() % interval_ns_);
                else if (updateClock)
                    lastRun_ns_ = NOW();
                return true;
            }
            else
                return false;
        }

        /**
         * If block is enabled then .isTimeToRun() will
         * always return false and .waitTillNextRun() will
         * never exit. This can be undone by calling the
         * method and giving false as a parameter.
         *
         * @param block true to block, false to not block
         */
        inline void block(bool block = true)
        {
            block_ = block;
        }
    };

}

#endif