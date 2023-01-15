#ifndef VECTRCORE_UTILITIES_SCHEDULER_H
#define VECTRCORE_UTILITIES_SCHEDULER_H

#include "list_array.hpp"
#include "time_system.hpp"

#include "string.h"
#include "stdint.h"

namespace VCTR
{

    /**
     * Gives a task a priority.
     * Lower priorities only run if no tasks exist at higher priorities.
     * Tasks with equal priorities will run one after another.
     * Tasks with REALTIME will always run. But must be written to run fast as to give
     * lower priority tasks time to run.
     */
    enum eTaskPriority_t : uint32_t
    {
        // Will only run if there is nothing to do.
        eTaskPriority_None = 0,
        // Will only run if nothing to do at higher priorities.
        eTaskPriority_VeryLow = 5000,
        // Will only run if nothing to do at higher priorities.
        eTaskPriority_Low = 10000,
        // Will only run if nothing to do at higher priorities.
        eTaskPriority_Middle = 15000,
        // Will only run if nothing to do at higher priorities.
        eTaskPriority_High = 20000,
        // Will only run if nothing to do at higher priorities. Recommended for I2C comms!
        eTaskPriority_VeryHigh = 25000,
        // Will always run once it needs to. Good for devices over SPI. I2C might be too slow use eTaskPriority_VeryHigh!
        eTaskPriority_Realtime = UINT32_MAX,
    };

    class Task_Threading
    {
    private:
        // Interval control for calculating system resource usage.
        static int64_t systemResourceCalcTimestamp_;

        static float schedulerUsage_;

        static float sleepPercent_;

        static ListArray<Task_Threading *> &taskList();

        // Function to run to enter sleep for given amount of time
        static void (*sleepFunction_)(int64_t);

        static int64_t timeSpentSleeping_;

        // Local variables each task has

        // Task name, up to 30 characters long
        char name_[30];

        // Interval to run task at.
        int64_t interval_ = 0;

        // Task Priority
        uint32_t priority_ = eTaskPriority_t::eTaskPriority_None;
        uint32_t priorityOffset_ = eTaskPriority_t::eTaskPriority_None;
        bool dynamicPriority_ = false;
        uint32_t priorityOffsetIncrement_ = 100;

        // When to run task thread
        int64_t nextRunTimestamp_ = 0;

        // If the task should be setup to run again in the given interval.
        bool intervalSchedulerEnabled_ = false;

        // If the task needs to be ran ignoring timing.
        bool forceTaskRun_ = false;

        // If the task should run. Used by scheduler
        bool runTask_ = false;

        // Counter for number of runs
        uint32_t runCounter_ = 0;

        // Timestamp for last run counter reset
        int64_t lastCounterTimestamp_ = 0;

        // Rate at which run is being called
        uint32_t runRate_ = 0;

        // Timestamps used to find out task time usage.
        float systemUsage_ = 0;
        int64_t systemTimeUsageCounter_ = 0;

    protected:
        // Whether init function was ran. Set to false for scheduler to run init again
        bool initWasCalled_ = false;

    public:
        /**
         * @brief Construct a new Task_Threading object
         *
         * @param taskName Name to give task
         * @param priority Priority to ran task at. Higher priority tasks are more likely to be ran.
         * @param dynamicPriority If enabled the an offset applied to priority will be incremented each miss and halfed each run, but will never reach realtime.
         * @param interval Interval to run task at. If set to 0 then the task will only be ran when suspendUntil() given time is reached.
         * @param startTime When to first run task
         */
        Task_Threading(const char *taskName, uint32_t priority, int64_t interval = 0, int64_t startTime = 0, bool dynamicPriority = false)
        {
            strncpy(name_, taskName, 30);
            priority_ = priority;
            dynamicPriority_ = dynamicPriority;
            setTaskInterval(interval);
            suspendUntil(startTime);
            taskList().append(this);
        }

        /**
         * Will remove Task from scheduler.
         */
        virtual ~Task_Threading()
        {
            this->removal();
            taskList().removeAllEqual(this); // Make sure it is removed.
        }

        /// ################ Below are functions that are to be implemented by subclasses and called by the scheduler system  ################

        /**
         * @brief This will be ran once before first run of thread()
         *
         */
        virtual void init(){};

        /**
         * @brief This will be ran at the given time
         *
         */
        virtual void thread() { suspendUntil(END_OF_TIME); };

        /**
         * @brief This this will ran only when the Task is removed from scheduler. E.g when the task is deleted.
         *
         */
        virtual void removal(){};

        /**
         * @brief Will be called by scheduler to make object update the next run if needed.
         * @note Can be overridden to let object check if it needs to run now and call suspendUntil(0)
         *
         */
        virtual void checkNextRun(){};

        /**
         * @brief This is called by the scheduler when the task is to be ran.
         *
         */
        void run()
        {

            if (intervalSchedulerEnabled_)
                suspendUntil(NOW() - NOW() % interval_ + interval_);
            else
                suspendUntil(END_OF_TIME);

            int64_t start = NOW(); // Needed to measure task time usage.
            thread();              // Run task.
            systemTimeUsageCounter_ += NOW() - start;

            runCounter_++;
        }

        /**
         * Will stop calling this thread until given time is reached. This is mainly used to control task timing
         *
         * @param time Time in nanoseconds at which the thread should continue
         */
        void suspendUntil(int64_t time)
        {

            nextRunTimestamp_ = time;
        }

        /// ################ Below are functions to get task information  ################

        /**
         * @brief Get the timestamp of when the task will be called again.
         *
         * @return int64_t
         */
        int64_t getNextTaskRun()
        {
            return nextRunTimestamp_;
        }

        /**
         * Returns task priority.
         *
         * @returns priority of type eTaskPriority_t.
         */
        uint32_t getTaskPriority()
        {
            return priority_;
        }

        /**
         * @brief Get the True Task Priority. Priority plus offset.
         *
         * @return uint32_t
         */
        uint32_t getTrueTaskPriority()
        {
            return priority_ + priorityOffset_;
        }

        /**
         * @brief Get the Task Interval in ns.
         *
         * @return int64_t
         */
        int64_t getTaskInterval()
        {
            return interval_;
        }

        /**
         * @brief Get the Task System Usage in percent 0-1.
         *
         * @return float
         */
        float getTaskSystemUsage()
        {
            return systemUsage_;
        }

        /**
         * @brief Get the amount of times the thread is called per second
         *
         * @return uint32_t
         */
        uint32_t getRunRate()
        {
            return runRate_;
        }

        /**
         * @brief Get the Task Name
         *
         * @return const char*
         */
        const char *getTaskName()
        {
            return name_;
        }

        /**
         * @brief Get the Scheduler System Usage
         *
         * @return float
         */
        static float getSchedulerSystemUsage()
        {
            return schedulerUsage_;
        }

        /**
         * @return Percent of time spent sleeping.
         */
        static float getSleepPercent()
        {
            return sleepPercent_;
        }

        /// ################ Below are functions to change task settings  ################

        /**
         * @brief Enables or disables the interval control of task scheduling.
         * The task be automatically be setup to run again in the setup interval.
         *
         * @param enable True to enable task scheduling
         */
        void enableSchedulingOfTask(bool enable)
        {
            intervalSchedulerEnabled_ = enable;
        }

        /**
         * Sets task rate to run at.
         *
         * @param rate is the rate to run at.
         */
        void setTaskRate(uint32_t rate) { interval_ = SECONDS / rate; }

        /**
         * Sets task rate to run at.
         *
         * @param rate is the rate to run at.
         */
        void setTaskInterval(int64_t interval)
        {
            interval_ = interval;
            enableSchedulingOfTask(interval_ > 0);
        }

        /**
         * Sets task priority.
         *
         * @param priority is of type uint32_t.
         */
        void setTaskPriority(uint32_t priority) { priority_ = priority; }

        /**
         * Static function to give internal scheduler time to run tasks.
         * This needs to be ran as often and fast as possible to give all tasks time.
         *
         * Usually the main program loop only constantly calls this.
         * Warning: Do not call this inside a task! Can cause stack overflow!
         */
        static void schedulerTick();

        /**
         * Static function that tells scheduler to initialize all attached tasks.
         */
        static void schedulerInitTasks();

        /**
         * @returns list containing pointers to all tasks.
         */
        static const ListArray<Task_Threading *> &getTaskList()
        {
            return taskList();
        }

        /**
         * @brief Sets the function to run for sleep.
         * @param sleepFunction Function to call. Set to nullptr to not sleep. Input param is the amount of time to sleep in ns.
         *
         */
        static void setSleepFunction(void (*sleepFunction)(int64_t))
        {
            sleepFunction_ = sleepFunction;
        }
    };

}

#endif
