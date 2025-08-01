#include "stddef.h"
#include "string.h"
#include "stdint.h"

#include "ExVectrCore/time_definitions.hpp"
#include "ExVectrCore/scheduler2.hpp"

#include "ExVectrCore/task_types.hpp"

VCTR::Core::Task_Periodic::Task_Periodic(const char *taskName, int64_t interval_ns, int64_t start, int64_t timeSlip_ns, bool skipOverdueRun) : Task(taskName)
{
    interval_ns_ = interval_ns;
    offset_ = start;
    skipOverdueRun_ = skipOverdueRun;

    setRelease(start);
    setDeadline(start + timeSlip_ns);
}

void VCTR::Core::Task_Periodic::setInterval(int64_t internal_ns)
{
    interval_ns_ = internal_ns;
}

int64_t VCTR::Core::Task_Periodic::getInterval()
{
    return interval_ns_;
}

void VCTR::Core::Task_Periodic::taskRun()
{
    if (skipOverdueRun_)
        deadline_ = NOW() - NOW() % interval_ns_ + interval_ns_;
    else
        deadline_ += interval_ns_;

    setRelease(deadline_);
    setDeadline(deadline_ + timeSlip_ns_);

    // VRBS_MSG("Task \"%s\" running app thread. \n", taskName_);
    taskThread(); // Run task.
    // VRBS_MSG("Task \"%s\" finished. \n", taskName_);
}