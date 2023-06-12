#ifndef EXVECTRCORE_SCHEDULER2_H
#define EXVECTRCORE_SCHEDULER2_H

#include "stddef.h"
#include "stdint.h"

#include "list_array.hpp"
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
                /// @brief if true then scheduler will not call run()
                bool taskPaused_ = false;
                /// @brief if false the scheduler will call init() ASAP
                bool taskInitisalised_ = false;
                /// @brief latest time the scheduler should call run()
                int64_t taskDeadline_ = 0;
                /// @brief earliest time the scheduler should call run()
                int64_t taskRelease_ = 0;
                /// @brief what the task priority is. Higher priorities are more likely to meet target timings for run().
                size_t taskPriority_ = 0;
                ///@brief Scheduler calling this task. 
                Scheduler* scheduler_ = nullptr;

            public:

                virtual ~Task();

                /**
                 * @returns a char array for the name of the task.
                 */
                virtual const char *taskName() = 0;

                /**
                 * Will be called by scheduler ASAP if isInitialised() returns false.
                 */
                virtual void taskInit() = 0;

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
                 * Latest time the task can be called.
                 */
                int64_t getDeadline();

                /**
                 * Latest time the task can be called.
                 */
                void setDeadline(int64_t deadline);

                /**
                 * Earliest time the task can be called.
                 */
                int64_t getRelease();

                /**
                 * Earliest time the task can be called.
                 */
                void setRelease(int64_t release);

                /**
                 * Priority of the task. Higher is more likely to hit timing targets.
                 */
                size_t getPriority();

                /**
                 * Priority of the task. Higher is more likely to hit timing targets.
                 */
                void setPriority(size_t priority);

                /**
                 * init() will be called by scheduler if this returns false.
                 */
                bool getInitialised();

                /**
                 * init() will be called by scheduler if false.
                 */
                void setInitialised(bool isInitialised);

                /**
                 * @returns if task is currently paused.
                 */
                bool getPaused();

                /**
                 * task will not run if true
                 */
                void setPaused(bool pause);
            };

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
             * Finds the task that needs to be ran next and returns its release time.
            */
            int64_t getNextTaskRelease();

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
            virtual int32_t getTaskPseudoPriority(const TaskData& task);

        };


        /**
         * A general system scheduler ordinary tasks can use this. Uses the internal accurate clock for timing.
        */
        extern Scheduler& getSystemScheduler();


    }

}

#endif
