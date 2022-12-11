#ifndef DEBUG_IO_H
#define DEBUG_IO_H


#include "KraftKontrol/utils/topic.h"


/**
 * Used for outputing errors, warnings and logging strings. 
 * Use given functions to notify users at runtime of failures, warnings or simply logging outputs.
 */



class DebugIO final {
public:

    ///Topic where logging strings are published.
    static Topic<const char*>& getLoggingTopic();

    ///Topic where warning strings are published.
    static Topic<const char*>& getWarningTopic();

    ///Topic where error strings are published.
    static Topic<const char*>& getErrorTopic();

    ///Topic where user command strings are published.
    static Topic<const char*>& getUserCommandTopic();

    /**
     * Publishes logging char strings to logging topic. Subscribed devices will receive the string.
     * Logging strings are only for user data e.g. vehicle arming, 
     * @param str Char pointer to 0 terminated string to be published to logging devices. 
     */
    static void loggingString(const char* str);

    /**
     * Publishes warning char strings to warning topic. Subscribed devices will receive the string.
     * Warning strings are only for when things might not function correctly and the user should be notified.
     * @param str Char pointer to 0 terminated string to be published to warning devices. 
     */
    static void warningString(const char* str);

    /**
     * Publishes error char strings to error topic. Subscribed devices will receive the string.
     * Error strings are only for when things have failed and the system will not function correctly.
     * @param str Char pointer to 0 terminated string to be published to error devices. 
     */
    static void errorString(const char* str);

    /**
     * Publishes user command char strings to user command topic. Subscribed devices will receive the string.
     * User command strings are only for when the user issues a command at runtime.
     * @param str Char pointer to 0 terminated string to be published to error devices. 
     */
    static void userCommandString(const char* str);


};


///Global function to print logging strings. Implemented with DebugIO class.
extern void logStr(const char* str);

///Global function to print warning strings. Implemented with DebugIO class.
extern void warnStr(const char* str);

///Global function to print error strings. Implemented with DebugIO class.
extern void errorStr(const char* str);

///Global function to print error strings. Implemented with DebugIO class.
extern void userCommandStr(const char* str);


#endif 
