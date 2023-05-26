#ifndef EXVECTRCORE_MESSAGE_H
#define EXVECTRCORE_MESSAGE_H

#include "stdint.h"

namespace VCTR
{

    namespace Core
    {

        /**
         * @brief Messages are general information objects used to categorise and track data types like commands, telemetry etc. over non deterministic channels.
         */
        class Message
        {
        private:
        public:
            /**
             * @brief Message types, could be commands, telemetry from different modules or devices. Values below 1000 are reserved for ExVectr.
             */
            enum class Mes_Type
            {
                Command = 0, /// General command.
                Telemetry    /// General telemetry
            };

            /**
             * @brief Data types used by messages. Values below 1000 are reserved for ExVectr.
             */
            enum class Data_Type
            {
                Integer = 0, /// Signed integer with 32 bits
                Decimal,     /// A 32 bit floating point datatype, CUSTOM not castable to float/double.
                Matrix       /// General matrix. First 2 bytes give the dimensions in uint8_t and the values are Decimal.
            };

            /**
             * @brief the type of message, e.g. commands, telemetry etc.
             * @return message type as uint16_t.
             */
            virtual uint16_t messageType() const = 0;

            /**
             * @brief the type of data, e.g. int, float, 3D vector, 4x4 matrix etc.
             * @return data type as uint16_t.
             */
            virtual uint16_t dataType() const = 0;

            /**
             * @brief the length of the data in bytes
             * @return data in bytes as uint16_t.
             */
            virtual uint16_t dataLength() const = 0;

            /**
             * @brief Writes the message raw data into the given buffer.
             * @param buffer pointer to data to receive data.
             * @param bufferSize size limit of the buffer.
             * @return The number of bytes written into the buffer. Is 0 if a failure occured like buffer being too small.
             */
            virtual uint16_t getData(void *buffer, uint16_t bufferSize) = 0;

            /**
             * @brief Sets the message data using the data in the given buffer.
             * @param buffer pointer to data to write into this message
             * @param bufferSize number of bytes in the given buffer.
             * @return The number of bytes written into the message. Is 0 if a failure occured.
             */
            virtual uint16_t setData(const void *buffer, uint16_t bufferSize) = 0;
        };

    }

}

#endif