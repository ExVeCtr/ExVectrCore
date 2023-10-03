#ifndef EXVECTRCORE_SCHEDULER2_H
#define EXVECTRCORE_SCHEDULER2_H

#include "stddef.h"
#include "stdint.h"

// #include "list_array.hpp"
#include "list_linked.hpp"
#include "time_source.hpp"
#include "time_base.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * New Scheduler design features:
         * - Measure task runtime
         * - Automatic schedule planning (Finished)
         * - No priorities, rather realtime importance. (Finished)
         * - Supports event triggers (Based off of events). This allows cascading tasks (Run if another has ran).
         * - Timing relative to precise or accurate clock. (Finished)
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
        public:
            class Task
            {
                friend Scheduler;

            protected:
                /// @brief rate in Hz at which the task is actually being called.
                float taskRate_ = 0;
                /// @brief an average of how long the task takes to run in ns.
                int64_t taskRuntime_ = 0;
                /// @brief if true then scheduler will not call run()
                bool taskPaused_ = false;
                /// @brief if false the scheduler will call init() ASAP
                bool taskInitisalised_ = false;
                /// @brief latest time the scheduler should call run()
                int64_t taskDeadline_ = 0;
                /// @brief earliest time the scheduler should call run()
                int64_t taskRelease_ = 0;
                /// @brief what the task priority is. Higher priorities are more likely to meet target timings for run().
                uint16_t taskPriority_ = 100;
                /// @brief Name of task. Max 49 characters
                char taskName_[50];

            private:
                /// @brief Use by scheduler to iterate through all attached tasks.
                ListLinked<Task *> taskListElement_;
                /// @brief how many times the task has been called.
                size_t runCounter = 0;
                /// @brief time in ns of the last reset.
                int64_t counterResetTimestamp = 0;
                /// @brief The pseudo priority of the task. Contains priority of timing, misses, and timeslot length.
                int32_t pseudoPriority = 0;
                /// @brief The number of times the task was not called past its release time.
                int32_t misses = 0; // The number of time the task could have ran but did not. Increments priority with every miss. Is reset when task is ran.
                ///@brief Scheduler calling this task.
                Scheduler *scheduler_ = nullptr;

            public:

                /**
                 * @brief Task ctor.
                 */
                Task();

                virtual ~Task();

                /**
                 * Will be called by scheduler ASAP if isInitialised() returns false.
                 */
                virtual void taskInit();

                /**
                 * To be implemented by application tasks. Will be called by run().
                 */
                virtual void taskThread() = 0;

                /**
                 * To be implemented by each task type. Called by scheduler to allow task type to plan next run. Not to be implemented by application tasks.
                 * @note Defaults to simply calling thread();
                 */
                virtual void taskRun();

                /**
                 * Scheduler will call this to allow the task to check if and when the task should run.
                 */
                virtual void taskCheck();

                /**
                 * @returns a char array for the name of the task.
                 */
                const char *getTaskName() const;

                /**
                 * Latest time the task can be called.
                 */
                int64_t getDeadline() const;

                /**
                 * Latest time the task can be called.
                 */
                void setDeadline(int64_t deadline);

                /**
                 * Earliest time the task can be called.
                 */
                int64_t getRelease() const;

                /**
                 * Earliest time the task can be called.
                 */
                void setRelease(int64_t release);

                /**
                 * Priority of the task. Higher is more likely to hit timing targets.
                 */
                uint16_t getPriority() const;

                /**
                 * Priority of the task. Higher is more likely to hit timing targets.
                 */
                void setPriority(uint16_t priority);

                /**
                 * init() will be called by scheduler if this returns false.
                 */
                bool getInitialised() const;

                /**
                 * init() will be called by scheduler if false.
                 */
                void setInitialised(bool isInitialised);

                /**
                 * @returns if task is currently paused.
                 */
                bool getPaused() const;

                /**
                 * task will not run if true
                 */
                void setPaused(bool pause);

                /**
                 * @returns the rate the task is actually being called at.
                 */
                float getRate() const;

                /**
                 * @returns the average time the task takes to run in ns.
                 */
                int64_t getRuntime() const;

                /**
                 * @returns the scheduler calling this task. nullptr if not attached to a scheduler.
                 */
                Scheduler const* getScheduler() const;

                /**
                 * @brief removes this task from the scheduler.
                 */
                void removeFromScheduler();

            };

        private:
            /// @brief List of all tasks attached to this scheduler.
            ListLinked<Task *> *tasks_;
            /// @brief list of tasks that need to be ran sorted to their pseudo priority
            // ListArray<size_t> taskIndexRun_;
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
            Scheduler(Clock_Source &clockSource);

            /**
             * Adds a task to the scheduler to be ran.
             * @param task The task to be added to the Scheduler.
             * @returns true if successful.
             */
            bool addTask(Task &task);

            /**
             * Removes the given task from the scheduler.
             * @note This will not delete the task. Tasks can also remove themselves from the scheduler.
             * @param task The reference to task to be removed from scheduler.
             * @returns true if found and removed. False if not found.
             */
            bool removeTask(Task &task);

            /**
             * @returns list of tasks currently attached to scheduler.
             */
            const List<Task *> &getTasks() const;

            /**
             * Finds the task that needs to be ran next and returns its release time.
             */
            int64_t getNextTaskRelease() const;

            /**
             * Main processing function. Sorts through tasks and calls the one with highest priority.
             * @note To be called as fast and often as possible to meet timing requirements
             */
            void tick();

        private:
            /**
             * Calculates the given tasks pseudo priority for use in scheduling.
             * Can be overridden by subclass to change priority calculation.
             * @param task
             * @returns the pseudo priority
             */
            virtual int32_t getTaskPseudoPriority(const Task& task);
        };

        /**
         * A general system scheduler ordinary tasks can use this. Uses the internal accurate clock for timing.
         */
        extern Scheduler &getSystemScheduler();

    }

}

#endif
