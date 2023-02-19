#include "ExVectrCore/task_types.hpp"

#include "stddef.h"
#include "string.h"
#include "stdint.h"

#include "ExVectrCore/scheduler2.hpp"
#include "ExVectrCore/time_definitions.hpp"

VCTR::Core::Task_Periodic::Task_Periodic(const char *taskName, int64_t interval_ns, int64_t start, int64_t timeSlip_ns, bool skipOverdueRun)
{
    interval_ns_ = interval_ns;
    deadline_ = start;
    skipOverdueRun_ = skipOverdueRun;

    strncpy(taskName_, taskName, 50);
    taskName_[49] = '\0'; //Make sure end.

}

const char *VCTR::Core::Task_Periodic::taskName()
{
    return taskName_;
}

void VCTR::Core::Task_Periodic::taskRun()
{
    if (skipOverdueRun_)
        deadline_ = NOW() - NOW() % interval_ns_ + interval_ns_;
    else
        deadline_ += interval_ns_;

    setRelease(deadline_);
    setDeadline(deadline_ + timeSlip_ns_);

    taskThread(); // Run task.
}