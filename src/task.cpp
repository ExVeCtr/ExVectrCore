#include "ExVectrCore/task.hpp"


void VCTR::Core::Task::taskRun() {
    taskThread();
}

void VCTR::Core::Task::taskCheck() {}

int64_t VCTR::Core::Task::getDeadline() {
    return taskDeadline_;
}

void VCTR::Core::Task::setDeadline(int64_t deadline) {
    taskDeadline_ = deadline;
    if (taskDeadline_ < taskRelease_) taskRelease_ = taskDeadline_;
}

int64_t VCTR::Core::Task::getRelease() {
    return taskRelease_;
}

void VCTR::Core::Task::setRelease(int64_t release) {
    taskRelease_ = release;
    if (taskRelease_ > taskDeadline_) taskDeadline_ = taskRelease_;
}

size_t VCTR::Core::Task::getPriority() {
    return taskPriority_;
}

void VCTR::Core::Task::setPriority(size_t priority) {
    taskPriority_ = priority;
}

bool VCTR::Core::Task::getInitialised() {
    return taskInitisalised_;
}

void VCTR::Core::Task::setInitialised(bool isInitialised) {
    taskInitisalised_ = isInitialised;
}

bool VCTR::Core::Task::getPaused() {
    return taskPaused_;
}

void VCTR::Core::Task::setPaused(bool pause) {
    taskPaused_ = pause;
}
