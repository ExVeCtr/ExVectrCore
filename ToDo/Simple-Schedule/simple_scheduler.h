#ifndef SIMPLE_SCHEDULER_H
#define SIMPLE_SCHEDULER_H



#include "KraftKontrol/utils/chain_buffer.h"
#include "interval_control.h"



/**
 * This is an interface used by simple schedule. 
 * Everything that inherets from this class can be attached onto a scheduler.
 */
class Thread_Interface {
public: 

    //This is ran once before first run of thread()
    virtual void init() = 0;

    //This is ran only when the Task is removed from scheduler.
    virtual void removal() = 0;

    //Is called by scheduler
    void run() {

        runCounter_++;

        if (NOW() - lastCounterTimestamp_ >= 1*SECONDS) {
            rate_ = (int64_t)runCounter_;//*SECONDS/(NOW() - lastCounterTimestamp_);
            runCounter_ = 0;
            lastCounterTimestamp_ = NOW();
        }

        thread();

    }

    /**
     * @returns the rate at which the task is being called at.
     */
    uint32_t getLoopRate() {return rate_;}


private:

    //This should be overloaded by task class
    virtual void thread() = 0;

    //Counter for number of runs
    uint32_t runCounter_ = 0;

    //Timestamp for last run counter reset
    int64_t lastCounterTimestamp_ = 0;

    //Rate at which runs is being called
    uint32_t rate_ = 0;



};



/**
 * This is a class that can be used to attach simple functions to a scheduler. 
 */
class GlobalThread final: public Thread_Interface {
public:

    /**
     * Used to construct a Thread class.
     * 
     * @param functionToRunOnInit is a pointer to a function that will be run once before thread
     * @param functionToRunOnThread is a pointer to a function that will be run on thread.
     * @param functionToRunOnRemoval is a function that is only ran once on tast removal. Default value is null and will then be ignored.
     */
    GlobalThread(void (*functionToRunOnInit)(void), void (*functionToRunOnThread)(void), void (*functionToRunOnRemoval)(void) = nullptr) {
        functionToRunOnInit_ = functionToRunOnInit;
        functionToRunOnThread_ = functionToRunOnThread;
        functionToRunOnRemoval_ = functionToRunOnRemoval;
    }

    //This is ran once before thread first run
    void init() override {
        if (functionToRunOnInit_ != nullptr) functionToRunOnInit_();
    }

    //This is ran at rate set in scheduler.
    void thread() override {
        if (functionToRunOnThread_ != nullptr) functionToRunOnThread_();
    }

    //This is ran only when the Task is removed from scheduler.
    void removal() override {
        if (functionToRunOnRemoval_ != nullptr) functionToRunOnRemoval_();
    }


private:
    
    //Pointer to init function
    void (*functionToRunOnInit_)(void) = nullptr;

    //Pointer to thread function
    void (*functionToRunOnThread_)(void) = nullptr;

    //Pointer to removal function
    void (*functionToRunOnRemoval_)(void) = nullptr;
    

};



/**
 * Gives a task a priority. 
 * Lower priorities only run if no tasks exist at higher priorities.
 * Tasks with equal priorities will run one after another.
 * Tasks with REALTIME will always run. But must be written to run fast as to give
 * lower priority tasks time to run.
 */ 
enum eTaskPriority_t {
    //Will only run if there is nothing to do.
    eTaskPriority_None,
    //Will only run if nothing to do at higher priorities.
    eTaskPriority_VeryLow,
    //Will only run if nothing to do at higher priorities.
    eTaskPriority_Low,
    //Will only run if nothing to do at higher priorities.
    eTaskPriority_Middle,
    //Will only run if nothing to do at higher priorities.
    eTaskPriority_High,
    //Will only run if nothing to do at higher priorities. Recommended for I2C comms!
    eTaskPriority_VeryHigh,
    //Will always run once it needs to. Good for devices over SPI. I2C might be too slow use eTaskPriority_VeryHigh!
    eTaskPriority_Realtime,
};



class Scheduler {
public:

    /**
     * This is the loop for the scheduler.
     * It will check for functions that need to be ran and run them.
     *
     * @param values none.
     * @return none.
     */
    void tick();

    /**
     * Calls all initialisation functions that are attached
     */
    void initializeTasks();

    /**
     * Gives other tasks a chance to run. Could cost a LOT of stack memory if many functions call yield.
     * Due to last in first out nature of this yield function, the first task calling yield might wait a long time till next task yield call is finished.
     * Only one task should use yield function as a delay!
     */
    void yield() {

        tick();

    }

    /**
     * This adds a function to the scheduler. 
     * 
     * e.g. attachTask(FunctionToBeCalled, 1000, eTaskPriority_t::eTaskPriority_realtime);
     *
     * @param function Pointer to Thread task to be ran.
     * @param rate_Hz Rate in Hz to run task at.
     * @param priority Priority to run task at.
     * @param startTime_ns Time when to start runnin thread
     * @param time_ns Time length to run task for
     */
    void attachTask(Thread_Interface* function, uint32_t rate_Hz, eTaskPriority_t priority, int64_t startTime_ns = 0, int64_t time_ns = 0);

    /**
     * This adds a function to the scheduler. 
     * Will run a function for a given number of loops
     * 
     * e.g. attachTask(FunctionToBeCalled, 1000, eTaskPriority_t::eTaskPriority_realtime);
     *
     * @param function Pointer to Thread task to be ran.
     * @param rate_Hz Rate in Hz to try to run task at.
     * @param priority Priority to run task at.
     * @param numberLoops Number of times to run task.
     * @param startTime_ns Time when to start runnin thread
     */
    void attachTaskForNumberLoops(Thread_Interface* function, uint32_t rate_Hz, eTaskPriority_t priority, uint32_t numberLoops, int64_t startTime_ns = 0);

    /**
     * This removes a function from the scheduler.
     * 
     * Returns false if function not found.
     *
     * @param function Pointer to task to remove.
     * @returns true if task found and removed.
     */
    bool detachTask(Thread_Interface* function);

    /**
     * Used to get how often per second tick() is called. Can be used to see how the systems performance is.
     * 
     * @returns tick rate.
     */
    uint32_t getTickRate() {return tickRate_;}


private:

    //Struct for comparing and storing function pointers that will be run. This is without parameters
    struct Task {

        Thread_Interface* thread;
        IntervalControl interval;

        //If true then do not run task.
        bool isSuspended = false;

        //To store whether init function was ran.
        bool initWasCalled = false;

        //If not -1 then this task will be removed once it reached its removal threshold.
        int64_t timeLength_ns = 0;
        int64_t startTime_ns = 0;
        bool timeLimited = false;

        //If set to 0 then no limit. Should be decremented every run.
        uint32_t numberRunsLeft = 0;

        //Set to true if currently running.
        bool isRunning = false;

        //Operator must be overloaded for the Chainbuffer to find the correct Task. Only the function pointers must be the same.
        bool operator == (Task b) {
            return thread == b.thread;
        }

    };

    /**
     * Runs through chain of tasks.
     * @param startOfGroup Pointer to starting task of chain.
     * @returns true if al least one task ran.
     */
    bool runPrioGroup(ChainObject<Task>* startOfGroup);

    ChainBuffer<Task> tasks_[7]; //array size must be as big, as the number of priorities there are.

    //Points to the thread the is currently running.
    Task* currentRunningTask_ = nullptr;

    //Incremented every tick
    uint32_t tickCounter_ = 0;
    //Stores loopRate
    uint32_t tickRate_ = 0;
    //Used to reset loopcounter and save Looprate
    IntervalControl tickCounterResetInterval_ = IntervalControl(1);


};





#endif
