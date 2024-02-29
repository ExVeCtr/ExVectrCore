#ifndef EXVECTRCORE_DATABUFFER_HPP
#define EXVECTRCORE_DATABUFFER_HPP

#include "stdint.h"
#include "stddef.h"
#include "math.h"

#include "list_buffer.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * DataBuffer is used to store and manipulate data in a buffer. Often used to save class state or send data over a network.
         * @note not to be confused with ListBuffer.
         * @tparam SIZE The max size of the buffer in bytes.
         */
        template <size_t SIZE>
        class DataBuffer
        {
        private:
            
            /// @brief The buffer to store data.
            uint8_t buffer_[SIZE];

            /// @brief The pointer to the current position in the buffer.
            size_t ptr_ = 0;

        public:

            DataBuffer();
            
            /**
             * @brief gets the maximum size of the buffer.
            */
            size_t size() const;

            /**
             * @brief sets the pointer to start the following read or write operation at the specified index[bytes].
            */
            void setPtr(size_t index);

            /**
             * @brief gets the pointer to the current position in the buffer. Can also be interpreted as the number of bytes written to the buffer.
            */
            size_t getPtr() const;

            /// @brief Writes a value to the buffer. The pointer is incremented by the size of the data.
            /// @note Float/Double data will be added in their raw form. Use writeFactor() to use a platform independent format at the cost of performance.
            template <typename TYPE>
            void write(const TYPE& data);

            /**
             * @brief Divides the given value by the factor and writes it to the buffer.
             * @note This is used to write floating point data in a platform independent format. 
            */
            void writeF(float data, int factor);

            /**
             * @brief Divides the given value by the factor and writes it to the buffer.
             * @note This is used to write floating point data in a platform independent format. 
            */
            void writeD(double data, int factor);

            /**
             * @brief Writes an array of data to the buffer. The pointer is incremented by the size of the data.
             * @param data The data to write.
             * @param size The size of the data in bytes.
            */
            void write(const void *data, size_t size);


            /**
             * @brief Reads a value from the buffer. The pointer is incremented by the size of the data.
             * @param data The data to read into.
            */
            template <typename TYPE>
            void read(TYPE& data);

            /**
             * @brief Reads a value from the buffer and multiplies it by the factor. The pointer is incremented by the size of the data.
             * @param data The data to read into.
             * @param factor The factor to multiply the data by.
            */
            void readF(float& data, int factor);

            /**
             * @brief Reads a value from the buffer and multiplies it by the factor. The pointer is incremented by the size of the data.
             * @param data The data to read into.
             * @param factor The factor to multiply the data by.
            */
            void readD(double& data, int factor);

            /**
             * @brief Reads an array of data from the buffer. The pointer is incremented by the size of the data.
             * @param data The data to read into.
             * @param size The size of the data in bytes.
            */
            void read(void *data, size_t size);

        };


        template <size_t SIZE>
        DataBuffer<SIZE>::DataBuffer() {}

        template <size_t SIZE>
        size_t DataBuffer<SIZE>::size() const
        {
            return SIZE;
        }

        template <size_t SIZE>
        void DataBuffer<SIZE>::setPtr(size_t index)
        {
            ptr_ = index;
        }

        template <size_t SIZE>
        size_t DataBuffer<SIZE>::getPtr() const
        {
            return ptr_;
        }

        template <size_t SIZE>
        template <typename TYPE>
        void DataBuffer<SIZE>::write(const TYPE& data)
        {
            write(&data, sizeof(TYPE));
        }

        template <size_t SIZE>
        void DataBuffer<SIZE>::writeF(float data, int factor)
        {
            float temp = data / factor;
            write(&temp, sizeof(float));
        }

        template <size_t SIZE>
        void DataBuffer<SIZE>::writeD(double data, int factor)
        {
            double temp = data / factor;
            write(&temp, sizeof(double));
        }

        template <size_t SIZE>
        void DataBuffer<SIZE>::write(const void *data, size_t size)
        {
            if (ptr_ + size > SIZE)
            {
                size = SIZE - ptr_;
            }

            memcpy(&buffer_[ptr_], data, size);
            ptr_ += size;
        }

        template <size_t SIZE>
        template <typename TYPE>
        void DataBuffer<SIZE>::read(TYPE& data)
        {
            read(&data, sizeof(TYPE));
        }

        template <size_t SIZE>
        void DataBuffer<SIZE>::readF(float& data, int factor)
        {
            float temp;
            read(&temp, sizeof(float));
            data = temp * factor;
        }

        template <size_t SIZE>
        void DataBuffer<SIZE>::readD(double& data, int factor)
        {
            double temp;
            read(&temp, sizeof(double));
            data = temp * factor;
        }

        template <size_t SIZE>
        void DataBuffer<SIZE>::read(void *data, size_t size)
        {
            if (ptr_ + size > SIZE)
            {
                size = SIZE - ptr_;
            }

            memcpy(data, &buffer_[ptr_], size);
            ptr_ += size;

        }

    }
} // namespace VCTR

#endif