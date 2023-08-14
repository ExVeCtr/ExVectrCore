#include "ExVectrCore/scheduler2.hpp"

#include "stddef.h"
#include "stdint.h"

#include "ExVectrCore/list.hpp"
#include "ExVectrCore/List_linked.hpp"
#include "ExVectrCore/time_definitions.hpp"
#include "ExVectrCore/print.hpp"

/// @brief The global system scheduler
VCTR::Core::Scheduler &VCTR::Core::getSystemScheduler()
{
    static VCTR::Core::Scheduler systemScheduler;
    return systemScheduler;
}

VCTR::Core::Scheduler::Scheduler()
{
    // Time source will automatically use internal system clock.
}

VCTR::Core::Scheduler::Scheduler(Clock_Source &clockSource)
{
    timeSource_.setClockSource(clockSource);
}

const VCTR::Core::List<VCTR::Core::Scheduler::Task *> &VCTR::Core::Scheduler::getTasks() const
{

    if (tasks_ == nullptr)
        return List<Task *>::empty();

    return *tasks_;
}

bool VCTR::Core::Scheduler::addTask(Scheduler::Task &task)
{

    if (tasks_ == nullptr)
        tasks_ = &task.taskListElement_;
    else
        tasks_->append(task.taskListElement_);

    return true;
}

bool VCTR::Core::Scheduler::removeTask(Scheduler::Task &task)
{

    // Iterate through the list and find if the task is in the list. Remove if found.
    auto list = tasks_;
    while (list != nullptr)
    {
        if (list == &task.taskListElement_)
        {

            if (list == tasks_)
                tasks_ = list->getNext();

            list->remove();
            return true;
        }
        list = list->getNext();
    }

    return false;
}

int32_t VCTR::Core::Scheduler::getTaskPseudoPriority(const VCTR::Core::Scheduler::Task &task)
{

    uint64_t den = task.taskRuntime_ + task.getDeadline() - task.getRelease();

    if (den == 0)
        den = 1;

    return static_cast<int32_t>((NOW() - task.getRelease() + (task.getPriority() + task.misses) * VCTR::Core::MICROSECONDS) / den);
}

int64_t VCTR::Core::Scheduler::getNextTaskRelease() const
{

    if (tasks_ == nullptr)
        return VCTR::Core::END_OF_TIME;

    int64_t earliest = VCTR::Core::END_OF_TIME;
    auto task = tasks_;
    while (task != nullptr)
    {
        if ((*task)[0]->getRelease() < earliest)
            earliest = (*task)[0]->getRelease();
        task = task->getNext();
    }

    return earliest;
}

void VCTR::Core::Scheduler::tick()
{

    if (tasks_ == nullptr)
        return;

    /**
     * - Iterate through the list of tasks
     * - Let each task update their state
     * - Update their pseudo priority
     * - Find the task with the highest pseudo priority that should run.
     * - Increment the misses counter for all tasks that should run. (The selected task to run is will be set to 0, once it has run.)
     */
    auto task = tasks_;
    auto highestPriority = VCTR::Core::END_OF_TIME;
    VCTR::Core::ListLinked<VCTR::Core::Scheduler::Task *>* highestPriorityTask = nullptr;
    while (task != nullptr) 
    {
        (*task)[0]->taskCheck();
        (*task)[0]->pseudoPriority = getTaskPseudoPriority(*(*task)[0]);
        
        if (NOW() > (*task)[0]->getRelease())
        {
            (*task)[0]->misses++;

            if ((*task)[0]->pseudoPriority > highestPriority)
            {
                highestPriority = (*task)[0]->pseudoPriority;
                highestPriorityTask = task;
            }
        }

        task = task->getNext();
    }

    if (highestPriorityTask != nullptr)
    {

        auto task = (*highestPriorityTask)[0];

        task->misses = 0;
        task->runCounter++;

        if (!task->task->getInitialised())
        {
            task->task->setInitialised(true); // Before init so the task can override this when initialising.
            task->task->taskInit();
        }

        int64_t taskStart = Core::NOW();
        task->task->taskRun();
        int64_t taskLength = Core::NOW() - taskStart;
        task->task->taskRuntime_ = task->task->taskRuntime_ * 0.98 + taskLength * 0.02;

        if (Core::NOW() - task->counterResetTimestamp >= 5 * Core::SECONDS)
        {
            float dTime = float(Core::NOW() - task->counterResetTimestamp) / Core::SECONDS;

            task->task->taskRate_ = float(task->runCounter) / dTime;
            task->counterResetTimestamp = Core::NOW();
            task->runCounter = 0;
        }
        
    }

    /*for (size_t i = 0; i < taskIndexRun_.size(); i++) {
        Core::printM("Taskrun %d, %d\n", taskIndexRun_[i], tasks_[taskIndexRun_[i]].pseudoPriority);
    }*/

}

// Scheduler::Taskabstract functions

VCTR::Core::Scheduler::Task::~Task()
{
    if (scheduler_ != nullptr)
        scheduler_->removeTask(*this);
}

void VCTR::Core::Scheduler::Task::taskRun()
{
    taskThread();
}

void VCTR::Core::Scheduler::Task::taskCheck() {}

int64_t VCTR::Core::Scheduler::Task::getDeadline() const
{
    return taskDeadline_;
}

void VCTR::Core::Scheduler::Task::setDeadline(int64_t deadline)
{
    taskDeadline_ = deadline;
    if (taskDeadline_ < taskRelease_)
        taskRelease_ = taskDeadline_;
}

int64_t VCTR::Core::Scheduler::Task::getRelease() const
{
    return taskRelease_;
}

void VCTR::Core::Scheduler::Task::setRelease(int64_t release)
{
    taskRelease_ = release;
    if (taskRelease_ > taskDeadline_)
        taskDeadline_ = taskRelease_;
}

size_t VCTR::Core::Scheduler::Task::getPriority() const
{
    return taskPriority_;
}

void VCTR::Core::Scheduler::Task::setPriority(size_t priority)
{
    taskPriority_ = priority;
}

bool VCTR::Core::Scheduler::Task::getInitialised() const
{
    return taskInitisalised_;
}

void VCTR::Core::Scheduler::Task::setInitialised(bool isInitialised)
{
    taskInitisalised_ = isInitialised;
}

bool VCTR::Core::Scheduler::Task::getPaused() const
{
    return taskPaused_;
}

void VCTR::Core::Scheduler::Task::setPaused(bool pause)
{
    taskPaused_ = pause;
}

float VCTR::Core::Scheduler::Task::getRate() const
{
    return taskRate_;
}

int64_t VCTR::Core::Scheduler::Task::getRuntime() const
{
    return taskRuntime_;
}