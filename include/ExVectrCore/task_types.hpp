#ifndef EXVECTRCORE_TASKTYPES_H
#define EXVECTRCORE_TASKTYPES_H

#include "stddef.h"
#include "stdint.h"

#include "task.hpp"
#include "time_definitions.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * Types of tasks to be implemented:
         * - Periodic
         * - Static (Ran once)
         * - Scheduler task (Cascaded schedulers)
         */

        /**
         * A task that controls timing to be ran periodically at a given rate.
         * Functions to be implemented by inhereting application task:
         *  - void init();
         *  - void thread();
         *
         * @see Task interface for more information on these functions.
         *
         */
        class Task_Periodic : public Task
        {
        private:
            /// @brief Interval at which the task should be ran at in nanoseconds.
            int64_t interval_ns_ = 0;
            /// @brief How much earlier/later the task can be ran.
            int64_t timeSlip_ns_ = 0;
            /// @brief When this task should run.
            int64_t deadline_ = 0;
            /// @brief Offsets the time to stay in tact with the starting time.
            int64_t offset_ = 0;
            /// @brief Will skip missed timings.
            bool skipOverdueRun_ = true;
            /// @brief Name of task
            char taskName_[50];

        public:
            /**
             * @param taskName Name of task upto 20 chars.
             * @param interval_ns Interval at which to run at periodically in nanoseconds.
             * @param start When the task will start running periodically.
             * @param timeSlip_ns How much later the task can be ran.
             * @param skipOverdueRun If true, then in the case the task has not ran for a few cycles, then the task will only run once. If false then the task will run the amount of times missed.
             */
            Task_Periodic(const char *taskName, int64_t interval_ns, int64_t start = NOW(), int64_t timeSlip_ns = 1 * MILLISECONDS, bool skipOverdueRun = true);

            /**
             * Set the task interval in nanoseconds.
            */
            void setInterval(int64_t internal_ns);
            
            /**
             * Does what is says. Returns the task interval in nanoseconds.
            */
            int64_t getInterval();

            /**
             * @returns a char array for the name of the task.
             */
            const char *taskName() override final;

            /**
             * Called by scheduler to allow task to do its thing. In this task type, the task plans the next run.
             * @see periodicRun() for the child class running.
             */
            void taskRun() override final;
            
        };

    }

}

#endif
