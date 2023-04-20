#ifndef EXVECTRCORE_PRINT_H
#define EXVECTRCORE_PRINT_H

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

    }

}

#endif
