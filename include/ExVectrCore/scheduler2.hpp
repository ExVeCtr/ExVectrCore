#ifndef EXVECTRCORE_SCHEDULER2_H
#define EXVECTRCORE_SCHEDULER2_H

#include "stddef.h"
#include "stdint.h"

#include "list_array.hpp"
#include "time_source.hpp"
#include "time_base.hpp"

#include "task.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * New Scheduler design features:
         * - Measure task runtime
         * - Automatic schedule planning
         * - No priorities, rather realtime importance.
         * - Same timing structure as old system. (suspendUntil)
         * - Supports event triggers (Based off of events). This allows cascading tasks (Run if another has ran).
         * - Timing relative to precise or accurate clock.
         * - Support for sleep.
         * - Multiple schedulers for multi-core. (Maybe this allows for support of precise or accurate clock?)
         *
         * - Maybe AI search for optimal task planning?
         *
         */

        /**
         * @brief Class to organize and run given tasks with optimal timing.
         */
        class Scheduler
        {
        private:
            struct TaskData
            {
                Task *task = nullptr;
                int32_t pseudoPriority = 0;
                int64_t avgTaskLength_ns = 0;
                int32_t misses = 0; //The number of time the task could have ran but did not. Increments priority with every miss. Is reset when task is ran.
                TaskData();
                TaskData(Task* t);
                TaskData &operator=(const TaskData &b);
                bool operator==(const TaskData &b);
            };

            /// @brief List of all tasks attached to this scheduler.
            ListArray<TaskData> tasks_;
            /// @brief list of tasks that need to be ran sorted to their pseudo priority
            ListArray<size_t> taskIndexRun_;
            /// @brief source of time.
            Time_Source timeSource_;

        public:

            /**
             * Standard constructor. Uses internal clock as time source
            */
            Scheduler();

            /**
             * @param clockSource What source of time to use.
            */
            Scheduler(Clock_Source& clockSource);

            /**
             * Adds an item to the scheduler to be ran. In this case a Task.
             * @param task The reference to task to be added to the Scheduler.
             * @returns true if successful.
             */
            bool addTask(Task &task);

            /**
             * Removes the given task from the scheduler.
             * @param task The reference to task to be removed from scheduler.
             * @returns true if found and removed. False if not found.
             */
            bool removeTask(Task &task);

            /**
             * @returns list of tasks currently attached to scheduler.
             */
            const ListArray<TaskData> &getTasks();

            /**
             * Main processing function. Sorts through tasks and calls the ones needing to be ran.
             * @note To be called as fast and often as possible to meet timing requirements
             * @returns when the next task is expected to run.
             */
            int64_t tick();

        private:

            /**
             * Calculates the given tasks pseudo priority for use in scheduling.
             * Can be overridden by subclass to change priority calculation.
             * @param task 
             * @returns the pseudo priority
            */
            virtual uint32_t getTaskPseudoPriority(const TaskData& task);

        };


        /**
         * A general system scheduler ordinary tasks can use this. Uses the internal accurate clock for timing.
        */
        extern Scheduler& getSystemScheduler();


    }

}

#endif
