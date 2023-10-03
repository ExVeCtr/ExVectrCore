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

    task.scheduler_ = this;

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
            task.scheduler_ = nullptr;
            return true;
        }
        list = list->getNext();
    }

    return false;
}

int32_t VCTR::Core::Scheduler::getTaskPseudoPriority(const VCTR::Core::Scheduler::Task &task)
{

    // Calculate the pseudo priority of the task using the following criteria:
    //  - Tighter scheduling between release and deadline results in higher priority.
    //  - Higher priority results in higher priority.
    //  - More misses results in higher priority.
    //  - Higher runtime results in lower priority.
    //  - Closer to the deadline results in higher priority.

    // Currently only using the first and third criteria.

    size_t criteria1 = SIZE_MAX / (task.getDeadline() - task.getRelease() + 1);
    size_t criteria2 = task.getPriority();
    size_t criteria3 = task.misses;
    size_t criteria4 = task.taskRuntime_;

    size_t criteria5 = 0;
    /*if (NOW() > task.getDeadline())
        return INT32_MAX;*/

    auto pseudoPriority = criteria1 + criteria3 * 10;
    if (pseudoPriority > INT32_MAX)
        pseudoPriority = INT32_MAX;

    return pseudoPriority;
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
     * - Update their pseudo priority and find the task with the highest pseudo priority that should run.
     * - Increment the misses counter for all tasks that should run. (The selected task to run is will be set to 0, once it has run.)
     * - Run the task with the highest pseudo priority.
     */
    auto task = tasks_;
    auto highestPriority = 0;
    VCTR::Core::ListLinked<VCTR::Core::Scheduler::Task *> *highestPriorityTask = nullptr;
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

        auto taskRun = (*highestPriorityTask)[0];

        taskRun->misses = 0;
        taskRun->runCounter++;

        if (!taskRun->getInitialised())
        {
            taskRun->setInitialised(true); // Before init so the task can override this when initialising.
            taskRun->taskInit();
        }

        int64_t taskStart = Core::NOW();
        taskRun->taskRun();
        int64_t taskLength = Core::NOW() - taskStart;
        taskRun->taskRuntime_ = taskRun->taskRuntime_ * 0.98 + taskLength * 0.02;

        if (Core::NOW() - taskRun->counterResetTimestamp >= 5 * Core::SECONDS)
        {
            float dTime = float(Core::NOW() - taskRun->counterResetTimestamp) / Core::SECONDS;

            taskRun->taskRate_ = float(taskRun->runCounter) / dTime;
            taskRun->counterResetTimestamp = Core::NOW();
            taskRun->runCounter = 0;
        }
    }

    /*for (size_t i = 0; i < taskIndexRun_.size(); i++) {
        Core::printM("Taskrun %d, %d\n", taskIndexRun_[i], tasks_[taskIndexRun_[i]].pseudoPriority);
    }*/
}

// Scheduler::Taskabstract functions

VCTR::Core::Scheduler::Task::Task()
{
    taskListElement_[0] = this;
    taskName_[0] = '\0';
}

VCTR::Core::Scheduler::Task::~Task()
{
    if (scheduler_ != nullptr)
        scheduler_->removeTask(*this);
}

void VCTR::Core::Scheduler::Task::taskInit()
{
}

void VCTR::Core::Scheduler::Task::taskRun()
{
    taskThread();
}

void VCTR::Core::Scheduler::Task::taskCheck() {}

const char *VCTR::Core::Scheduler::Task::getTaskName() const
{
    return taskName_;
}

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

uint16_t VCTR::Core::Scheduler::Task::getPriority() const
{
    return taskPriority_;
}

void VCTR::Core::Scheduler::Task::setPriority(uint16_t priority)
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

VCTR::Core::Scheduler const *VCTR::Core::Scheduler::Task::getScheduler() const
{
    return scheduler_;
}

void VCTR::Core::Scheduler::Task::removeFromScheduler()
{
    if (scheduler_ != nullptr)
        scheduler_->removeTask(*this);
}