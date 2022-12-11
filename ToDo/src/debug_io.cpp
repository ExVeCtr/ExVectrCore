#include "KraftKontrol/utils/debug_io.h"


Topic<const char*>& DebugIO::getLoggingTopic() {

    static Topic<const char*> loggingTopic;
    return loggingTopic;

}

Topic<const char*>& DebugIO::getWarningTopic() {

    static Topic<const char*> warningTopic;
    return warningTopic;

}

Topic<const char*>& DebugIO::getErrorTopic() {

    static Topic<const char*> errorTopic;
    return errorTopic;

}

Topic<const char*>& DebugIO::getUserCommandTopic() {

    static Topic<const char*> userCommandTopic;
    return userCommandTopic;

}

void DebugIO::loggingString(const char* str) {
    getLoggingTopic().publish(str);
}

void DebugIO::warningString(const char* str) {
    getWarningTopic().publish(str);
}

void DebugIO::errorString(const char* str) {
    getErrorTopic().publish(str);
}

void DebugIO::userCommandString(const char* str) {
    getUserCommandTopic().publish(str);
}


void logStr(const char* str) {
    DebugIO::loggingString(str);
}

void warnStr(const char* str) {
    DebugIO::warningString(str);
}

void errorStr(const char* str) {
    DebugIO::errorString(str);
}
