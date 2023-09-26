#ifndef EXVECTRCORE_PRINT_H
#define EXVECTRCORE_PRINT_H

#define EXVECTR_DEBUG_ENABLE
//#define EXVECTR_DEBUG_VRBS_ENABLE

#include "stddef.h"
#include "stdint.h"
#include "stdarg.h"

#include "topic.hpp"

namespace VCTR
{

    namespace Core
    {   

        /**
         * Topic that will publish messages.
        */
        Topic<const char*>& getMessageTopic();

        /**
         * Topic that will publish messages.
        */
        Topic<const char*>& getDebugTopic();

        /**
         * Topic that will publish warnings.
        */
        Topic<const char*>& getWarningTopic();

        /**
         * Topic that will publish errors.
        */
        Topic<const char*>& getErrorTopic();

        
        /**
         * Prints the given string and arguments to given topic.
        */
        void printTopic(VCTR::Core::Topic<const char*>& topic, const char* c_str, ...);

        /**
         * Prints a message to whatever output is setup as console.
         * @note Works similar to printf(format, values...)
        */
        void printM(const char* format, ...);

        /**
         * Prints a debug message to whatever output is setup as console.
         * @note Works similar to printf(format, values...)
        */
        void printD(const char* format, ...);

        /**
         * Prints a warning to whatever output is setup as console.
         * @note Works similar to printf(format, values...)
        */
        void printW(const char* format, ...);

        /**
         * Prints an error to whatever output is setup as console.
         * @note Works similar to printf(format, values...)
        */
        void printE(const char* format, ...);


        #ifdef EXVECTR_DEBUG_ENABLE
        #define LOG_MSG(...) {VCTR::Core::printD("LOG from %s, %d, %s: ", __FILE__, __LINE__, __FUNCTION__); VCTR::Core::printD(__VA_ARGS__);} // Use this to print debugging messages.
        #else
        #define LOG_MSG(...)                     // Use this to print debugging messages. CURRENTLY DISABLED
        #endif

        #ifdef EXVECTR_DEBUG_VRBS_ENABLE
        #define VRBS_MSG(...) {VCTR::Core::printD("VERBOSE from %s, %d, %s: ", __FILE__, __LINE__, __FUNCTION__); VCTR::Core::printD(__VA_ARGS__);} // Use this to print debugging messages.
        #else
        #define VRBS_MSG(...)                     // Use this to print debugging messages. CURRENTLY DISABLED
        #endif


    }

}

#endif
