#include "ExVectrCore/scheduler2.hpp"

#include "stddef.h"
#include "stdint.h"

#include "ExVectrCore/list_array.hpp"
#include "ExVectrCore/time_definitions.hpp"
#include "ExVectrCore/print.hpp"

/// @brief The global system scheduler
VCTR::Core::Scheduler &VCTR::Core::getSystemScheduler()
{
    static VCTR::Core::Scheduler systemScheduler;
    return systemScheduler;
}

VCTR::Core::Scheduler::TaskData::TaskData()
{
    task = nullptr;
    pseudoPriority = 0;
    avgTaskLength_ns = 0;
    misses = 0;
}

VCTR::Core::Scheduler::TaskData::TaskData(Task *t)
{
    task = t;
    pseudoPriority = 0;
    avgTaskLength_ns = 0;
    misses = 0;
}

bool VCTR::Core::Scheduler::TaskData::operator==(const TaskData &b)
{
    return task == b.task;
}

VCTR::Core::Scheduler::TaskData &VCTR::Core::Scheduler::TaskData::operator=(const TaskData &b)
{
    avgTaskLength_ns = b.avgTaskLength_ns;
    pseudoPriority = b.pseudoPriority;
    misses = b.misses;
    task = b.task;
    return *this;
}

VCTR::Core::Scheduler::Scheduler()
{
    // Time source will automatically use internal system clock.
}

VCTR::Core::Scheduler::Scheduler(Clock_Source &clockSource)
{
    timeSource_.setClockSource(clockSource);
}

const VCTR::Core::ListArray<VCTR::Core::Scheduler::TaskData> &VCTR::Core::Scheduler::getTasks()
{
    return tasks_;
}

bool VCTR::Core::Scheduler::addTask(Task &task)
{
    tasks_.appendIfNotInListArray(TaskData(&task));

    if (tasks_.appendIfNotInListArray(TaskData(&task)))
    {
        task.scheduler_ = this;
        return true;
    }

    return false;
}

bool VCTR::Core::Scheduler::removeTask(Task &task)
{

    bool found = false;

    for (size_t i = 0; i < tasks_.size(); i++)
    {

        if (tasks_[i].task == &task)
        {
            found = true;
            tasks_.removeAtIndex(i);
            taskIndexRun_.removeAllEqual(i);
            i--;
        }
    }

    if (found)
        task.scheduler_ = nullptr;

    return found;
}

int32_t VCTR::Core::Scheduler::getTaskPseudoPriority(const VCTR::Core::Scheduler::TaskData &task)
{

    uint64_t den = task.avgTaskLength_ns + task.task->getDeadline() - task.task->getRelease();

    if (den == 0)
        den = 1;

    return static_cast<int32_t>((NOW() - task.task->getRelease() + (task.task->getPriority() + task.misses) * VCTR::Core::MICROSECONDS) / den);
}

int64_t VCTR::Core::Scheduler::getNextTaskRelease()
{

    int64_t earliest = VCTR::Core::END_OF_TIME;
    for (size_t i = 0; i < tasks_.size(); i++)
    {
        if (tasks_[i].task->getRelease() < earliest)
            earliest = tasks_[i].task->getRelease();
    }

    return earliest;
}

void VCTR::Core::Scheduler::tick()
{

    // Let tasks check their runtime.
    for (size_t i = 0; i < tasks_.size(); i++)
    {
        if (tasks_[i].task == nullptr)
        {
            VCTR::Core::printE("Scheduler with a nullptr task! Removing!");
            tasks_.removeAtIndex(i);
            i--;
            continue;
        }
        tasks_[i].task->taskCheck();
    }

    for (size_t i = 0; i < tasks_.size(); i++) // Add only the tasks that need to be ran
    {

        if (!tasks_[i].task->getPaused() && NOW() > tasks_[i].task->getRelease())
        {

            taskIndexRun_.appendIfNotInListArray(i);
        }
    }

    for (size_t i = 0; i < taskIndexRun_.size(); i++) // Update task priorities
    {
        tasks_[taskIndexRun_[i]].pseudoPriority = getTaskPseudoPriority(tasks_[taskIndexRun_[i]]);
    }

    // Sort by pseudo priority. Yes this is insertion sort with a runtime of O(n^2) but since the array is kept and not changed much between runs
    // then insertion sort has a general runtime of o(n) when almost sorted.
    for (size_t i = 1; i < taskIndexRun_.size(); i++)
    {
        size_t index = taskIndexRun_[i];
        auto prio = tasks_[index].pseudoPriority;
        size_t j = i;
        while (j > 0 && tasks_[taskIndexRun_[j - 1]].pseudoPriority > prio)
        {
            taskIndexRun_[j] = taskIndexRun_[j - 1];
            j--;
        }
        taskIndexRun_[j] = index;
    }

    /*for (size_t i = 0; i < taskIndexRun_.size(); i++) {
        Core::printM("Taskrun %d, %d\n", taskIndexRun_[i], tasks_[taskIndexRun_[i]].pseudoPriority);
    }*/

    if (taskIndexRun_.size() > 0)
    { // Run highest task if there is one.

        TaskData& taskRun = tasks_[taskIndexRun_[taskIndexRun_.size() - 1]];

        if (!taskRun.task->getInitialised())
        {
            taskRun.task->setInitialised(true); // Before init so the task can override this when initialising.
            taskRun.task->taskInit();
        }

        tasks_[taskIndexRun_[taskIndexRun_.size() - 1]].misses = 0;
        taskIndexRun_.removeAtIndex(taskIndexRun_.size() - 1); // Should be efficient, as item is at back of list.

        taskRun.task->taskRun();

        // Increase misses counter for all tasks.
        for (size_t i = 0; i < taskIndexRun_.size(); i++)
        {
            tasks_[taskIndexRun_[i]].misses++;
        }
    }
}