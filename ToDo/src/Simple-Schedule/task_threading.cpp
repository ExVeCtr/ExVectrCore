#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"


#include "Arduino.h"


int64_t Task_Threading::systemResourceCalcTimestamp_;
float Task_Threading::schedulerUsage_;
float Task_Threading::sleepPercent_ = 0;
void (*Task_Threading::sleepFunction_)(int64_t) = nullptr;
int64_t Task_Threading::timeSpentSleeping_ = 0;



void Task_Threading::schedulerInitTasks() {

    for (uint32_t i = 0; i < taskList().getNumItems(); i++) {
        taskList()[i]->initWasCalled_ = true;
        taskList()[i]->init();
    }

}


void Task_Threading::schedulerTick() {

    if (taskList().getNumItems() == 0) return;

    uint32_t lastPriority = UINT32_MAX;
    uint32_t currentPriority = UINT32_MAX;

    bool taskRan = false;
    bool taskFound = true;

    Task_Threading* task;

    for (uint32_t i = 0; i < taskList().getNumItems(); i++) {
        task = taskList()[i];
        if (!task->initWasCalled_) {
            task->initWasCalled_ = true;
            task->init();
        }
        task->checkNextRun();
        task->runTask_ = NOW() >= task->nextRunTimestamp_;
    }


    while (!taskRan && taskFound) {

        //Check if any tasks with this priority need to be ran
        for (uint32_t i = 0; i < taskList().getNumItems(); i++) {

            task = taskList()[i];

            //Only run a task if its not suspended and priority is same as one to run.
            if (task->getTrueTaskPriority() == currentPriority && task->runTask_) {

                taskRan = true;

                task->run();

                task->runTask_ = false;

                if (task->dynamicPriority_) {
                    task->priorityOffset_ /= 2;
                }

            }

            //yield(); //Should be called as this could take a while.

        }

        
        //Find the next lower highest priority in task list
        if (!taskRan) {

            lastPriority = currentPriority;
            currentPriority = 0;

            taskFound = false;

            for (uint32_t i = 0; i < taskList().getNumItems(); i++) {

                task = taskList()[i];

                if (task->getTrueTaskPriority() > currentPriority && task->getTrueTaskPriority() < lastPriority) {
                    currentPriority = task->getTrueTaskPriority();
                    taskFound = true;
                }

            }

        } else {

            uint32_t taskPrio = 0;

            for (uint32_t i = 0; i < taskList().getNumItems(); i++) {

                task = taskList()[i];

                taskPrio = task->getTrueTaskPriority();

                if (task->dynamicPriority_ && taskPrio < currentPriority && task->runTask_ && taskPrio < (eTaskPriority_t::eTaskPriority_Realtime - task->priorityOffsetIncrement_)) task->priorityOffset_ += task->priorityOffsetIncrement_;

                if (taskPrio >= eTaskPriority_t::eTaskPriority_Realtime) task->priorityOffset_ = eTaskPriority_t::eTaskPriority_Realtime - 1;

            }
            
        }

    }


    int64_t dTime = NOW() - systemResourceCalcTimestamp_;
    if (dTime >= 5*SECONDS) {
        systemResourceCalcTimestamp_ = NOW() + 5*SECONDS;

        //Calculate time usage for each task.
        int64_t totalTime = dTime;  
        int64_t timeLeft = totalTime;
        for (uint32_t i = 0; i < taskList().getNumItems(); i++) {

            task = taskList()[i];

            task->systemUsage_ = (float)task->systemTimeUsageCounter_/totalTime;
            timeLeft -= task->systemTimeUsageCounter_;
            task->systemTimeUsageCounter_ = 0;

            task->runRate_ = task->runCounter_/((float)dTime/SECONDS);
            task->runCounter_ = 0;

        }

        schedulerUsage_ = (float)(totalTime - timeLeft)/totalTime;
        sleepPercent_ = (float)timeSpentSleeping_/dTime;

        timeSpentSleeping_ = 0;

    }


    int64_t nextTaskRun = END_OF_TIME;
    for (uint32_t i = 0; i < taskList().getNumItems(); i++) {

        task = taskList()[i];

        if (task->runTask_) {

            nextTaskRun = NOW();

            break;

        }

        if (task->nextRunTimestamp_ < nextTaskRun) nextTaskRun = task->nextRunTimestamp_;

    }

    //Serial.print("Time: ");
    //Serial.println((double)(nextTaskRun - NOW())/MILLISECONDS);

    if (sleepFunction_ != nullptr && nextTaskRun != END_OF_TIME && nextTaskRun - NOW() > 2*MILLISECONDS) {

        int64_t sleepTime = nextTaskRun - NOW() - 1*MILLISECONDS;
        int64_t start = NOW();
        sleepFunction_(sleepTime);
        timeSpentSleeping_ += NOW() - start;

    }


}



List<Task_Threading*>& Task_Threading::taskList() {

    static List<Task_Threading*> taskList_g = List<Task_Threading*>();

    return taskList_g;

}