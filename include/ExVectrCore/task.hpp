#ifndef EXVECTRCORE_TASK_H
#define EXVECTRCORE_TASK_H

#include "stddef.h"
#include "stdint.h"

#include "list_array.hpp"

namespace VCTR
{

    namespace Core
    {
        //Forward declaration of scheduler
        class Scheduler;

        /**
         * Abstract class for tasks to be called by scheduler.
         * Init() is supposed to initialise the Task and run() give the task time to do its thing.
         * deadline and release give the earliest and latest times to run the task.
         *
         * @see task_types.hpp for implementations of periodic tasks, event based etc.
         *
         */
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

    }

}

#endif
