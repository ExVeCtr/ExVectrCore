#ifndef VECTRCORE_BUFFER_H
#define VECTRCORE_BUFFER_H

#include "stdint.h"
#include "math.h"

namespace VCTR
{

    /**
     * Buffer class that can be used as queue or stack.
     * Can also be used to sort values and calculate median, average, deviation.
     */
    template <typename T, size_t size_>
    class Buffer
    {
    private:
        // Array for element storage
        T bufferArray_[size_];

        // Should always have number of elements.
        size_t numElements_ = 0;

        // Points to index ahead of first element
        size_t front_ = 0;
        // Points to index of last element
        size_t back_ = 0;

    public:
        /**
         *
         */
        Buffer() {}

        ~Buffer() {} // Remember to delete data on destruction

        /**
         * @returns number of elements in buffer
         */
        inline size_t available() const;

        /**
         * @returns number of empty spaces that can be filled.
         */
        inline size_t availableSpace() const;

        /**
         * Places a new element to the front of the buffer.
         *
         * @param element element to be placed into buffer.
         * @param overwrite Overwrites elements at back if true. Default false.
         * @return true if placed into buffer.
         */
        inline bool placeFront(const T &element, bool overwrite = false);

        /**
         * Places a new element to the front of the buffer.
         *
         * @param element element to be placed into buffer.
         * @param overwrite Overwrites elements at back if true. Default false.
         * @return true if placed into buffer.
         */
        inline bool placeFront(const T &&element, bool overwrite = false);

        /**
         * Places a new element to the back of the buffer.
         *
         * @param element element to be placed into buffer.
         * @param overwrite Overwrites elements at front if true. Default false.
         * @return true if placed into buffer.
         */
        inline bool placeBack(const T &element, bool overwrite = false);

        /**
         * Places a new element to the back of the buffer.
         *
         * @param element element to be placed into buffer.
         * @param overwrite Overwrites elements at front if true. Default false.
         * @return true if placed into buffer.
         */
        inline bool placeBack(const T &&element, bool overwrite = false);

        /**
         * Takes a element from the front of the buffer and places it into element.
         * peekFront() wont remove element.
         * Calling this on an empty buffer will simply return false.
         *
         * @param element Variable whos data will be overwritten.
         * @return true if removed from buffer.
         */
        inline bool takeFront(T &element);

        /**
         * Takes a element from the back of the buffer and places it into element.
         * peekBack() wont remove element.
         * Calling this on an empty buffer will simply return false.
         *
         * @param element Variable whos data will be overwritten.
         * @return true if removed from buffer.
         */
        inline bool takeBack(T &element);

        /**
         * Copies a element from the front of the buffer and places it into element.
         * takeFront() will remove element.
         * Calling this on an empty buffer will simply return false.
         *
         * @param element Variable whos data will be overwritten.
         * @return true if removed from buffer.
         */
        inline bool peekFront(T &element);

        /**
         * Copies a element from the back of the buffer and places it into element.
         * takeBack() will remove element.
         * Calling this on an empty buffer will simply return false.
         *
         * @param element Variable whos data will be overwritten.
         * @return true if removed from buffer.
         */
        inline bool peekBack(T &element);

        /**
         * Removes the element at the front.
         * Calling this on an empty buffer will do nothing.
         */
        inline void removeFront();

        /**
         * Removes the element at the back.
         * Calling this on an empty buffer will do nothing.
         */
        inline void removeBack();

        /**
         * Removes element that the given pointer points to.
         * @param pointerToElement Pointer to element to remove.
         * @returns true if element was found and removed.
         */
        inline bool removeElement(T *pointerToElement);

        /**
         * Removes element at given index.
         * @param index Index of element to be removed.
         * @returns true if element was found and removed.
         */
        inline bool removeElementIndex(int32_t index);

        /**
         * Places element into given index
         * @param index Index of position to place element
         * @returns true if element was placed in buffer.
         */
        // bool insertElementIndex(const T &element, size_t index);

        /**
         * @returns the sum of all elements.
         */
        inline T getSum() const;

        /**
         * @returns the average of all elements.
         */
        inline T getAverage() const;

        /**
         * Sorts the elements. This will change the array!
         */
        inline void sortElements();

        /**
         * Sorts elements and returns median.
         * @return median.
         */
        inline T getMedian() const;

        /**
         * @returns the standard deviation.
         */
        inline T getStandardDeviation() const;

        /**
         * @returns the standard error.
         */
        inline T getStandardError() const;

        /**
         * Used to access buffer like an array.
         * Indexes outside of buffer will wrap around buffer.
         * Starts from back of buffer("oldest" element or first that was placed inside).
         * @returns copy of element from index.
         */
        inline T &operator[](int32_t index);

        /**
         * Used to access buffer like an array.
         * Const version. Returns a const reference to element.
         * Indexes outside of buffer will wrap around buffer.
         * Starts from back of buffer("oldest" element or first that was placed inside).
         * @returns copy of element from index.
         */
        inline const T &operator[](int32_t index) const;

        /**
         * Needs to be overloaded to also copy the data to instance.
         * Not doing this will cause 2 buffers to share the exact same elements.
         */
        inline Buffer operator=(const Buffer &toBeCopied);

        /**
         * Removes all items from buffer. Not computationaly intensive.
         */
        inline void clear();

    private:
        inline void quickSort(size_t left, size_t right);

        inline size_t quickSortPartition(size_t left, size_t right);
    };

    template <typename T, size_t size_>
    inline T Buffer<T, size_>::getStandardError() const
    {
        if (numElements_ < 2)
            return T();
        return getStandardDeviation() / sqrt(numElements_);
    }

    template <typename T, size_t size_>
    inline T Buffer<T, size_>::getStandardDeviation() const
    {

        if (numElements_ < 2)
            return T();

        T standardDev = 0;

        T avg = getAverage();

        for (size_t i = 0; i < numElements_; i++)
        {

            T diff = (*this)[i] - avg;

            standardDev = standardDev + diff * diff;
        }

        standardDev = sqrt(standardDev / (numElements_ - 1));

        return standardDev;
    }

    template <typename T, size_t size_>
    inline T Buffer<T, size_>::getMedian() const
    {

        if (numElements_ == 0)
            return T();

        T median;

        Buffer<T, size_> bufferSorted;

        bufferSorted = *this;

        bufferSorted.sortElements();

        if (numElements_ % 2 == 0)
        {

            median = (bufferSorted[numElements_ / 2 - 1] + bufferSorted[numElements_ / 2]) / 2;
        }
        else
        {

            median = bufferSorted[numElements_ / 2];
        }

        return median;
    }

    template <typename T, size_t size_>
    inline T Buffer<T, size_>::getAverage() const
    {
        if (numElements_ == 0)
            return T();
        return getSum() / numElements_;
    }

    template <typename T, size_t size_>
    inline T Buffer<T, size_>::getSum() const
    {

        T sum_ = 0;

        for (size_t i = 0; i < numElements_; i++)
        {
            sum_ = sum_ + (*this)[i];
        }

        return sum_;
    }

    template <typename T, size_t size_>
    inline size_t Buffer<T, size_>::quickSortPartition(size_t left, size_t right)
    {

        T pivot = (*this)[right];

        size_t i = left;

        for (size_t j = left; j < right; j++)
        {

            if ((*this)[j] < pivot)
            {

                // swap
                T temp = (*this)[i];
                (*this)[i] = (*this)[j];
                (*this)[j] = temp;

                i++;
            }
        }

        // swap
        T temp = (*this)[i];
        (*this)[i] = (*this)[right];
        (*this)[right] = temp;

        return i;
    }

    template <typename T, size_t size_>
    inline void Buffer<T, size_>::quickSort(size_t left, size_t right)
    {

        if (left < right)
        {

            size_t p = quickSortPartition(left, right);
            if (p != left)
                quickSort(left, p - 1);
            quickSort(p + 1, right);
        }
    }

    template <typename T, size_t size_>
    inline void Buffer<T, size_>::sortElements()
    {

        // Check if nothing to sort
        if (numElements_ < 2)
            return;

        quickSort(0, numElements_ - 1);
    }

    template <typename T, size_t size_>
    inline void Buffer<T, size_>::clear()
    {
        front_ = back_ = numElements_ = 0;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::removeElementIndex(int32_t index)
    {

        // Make sure buffer isnt empty
        if (numElements_ == 0)
            return false;

        // Special case if numberElements if 1. Algorithm below cant handle this case.
        if (numElements_ == 1)
        {
            clear();
            return true;
        }

        // We have to move all elements ahead the one to be removed, one place down.
        for (size_t j = index; j < numElements_ - 1; j++)
        {

            ((*this)[j]) = ((*this)[j + 1]);
        }

        numElements_--;

        // If we exited the loop then item was not found.
        return true;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::removeElement(T *pointerToElement)
    {

        // Make sure buffer isnt empty
        if (numElements_ == 0)
            return false;

        // Find element it points to
        for (size_t i = 0; i < numElements_; i++)
        {

            // Check if this is the item
            if (&((*this)[i]) == pointerToElement)
            {

                return removeElementIndex(i);
            }
        }

        // If we exited the loop then item was not found.
        return false;
    }

    template <typename T, size_t size_>
    inline T &Buffer<T, size_>::operator[](int32_t index)
    {
        if (index < 0)
            index = numElements_ + index;
        return bufferArray_[(back_ + index) % numElements_];
    }

    template <typename T, size_t size_>
    inline const T &Buffer<T, size_>::operator[](int32_t index) const
    {
        if (index < 0)
            index = numElements_ + index;
        return bufferArray_[(back_ + index) % numElements_];
    }

    template <typename T, size_t size_>
    inline Buffer<T, size_> Buffer<T, size_>::operator=(const Buffer &toBeCopied)
    {

        size_t sizeToBeCopied = toBeCopied.available();

        for (size_t i = 0; i < sizeToBeCopied; i++)
            this->placeFront(toBeCopied[i]);

        return *this;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::peekFront(T &element)
    {

        if (numElements_ == 0)
            return false;

        element = bufferArray_[(front_ + 1) % size_];

        return true;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::peekBack(T &element)
    {

        if (numElements_ == 0)
            return false;

        element = bufferArray_[back_];

        return true;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::takeFront(T &element)
    {

        if (numElements_ == 0)
            return false;

        if (front_ == 0)
            front_ = size_ - 1;
        else
            front_--;

        element = bufferArray_[front_];

        numElements_--;

        return true;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::takeBack(T &element)
    {

        if (numElements_ == 0)
            return false;

        element = bufferArray_[back_];

        back_ = (back_ + 1) % size_;

        numElements_--;

        return true;
    }

    template <typename T, size_t size_>
    inline size_t Buffer<T, size_>::available() const
    {
        return numElements_;
    }

    template <typename T, size_t size_>
    inline size_t Buffer<T, size_>::availableSpace() const
    {
        return size_ - numElements_;
    }

    template <typename T, size_t size_>
    inline void Buffer<T, size_>::removeFront()
    {

        if (numElements_ == 0)
            return;

        if (front_ == 0)
            front_ = size_ - 1;
        else
            front_--;
        numElements_--;
    }

    template <typename T, size_t size_>
    inline void Buffer<T, size_>::removeBack()
    {

        if (numElements_ == 0)
            return;

        back_ = (back_ + 1) % size_;
        numElements_--;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::placeFront(const T &element, bool overwrite)
    {

        if (numElements_ == size_)
        {

            if (overwrite)
                removeBack();
            else
                return false;
        }

        bufferArray_[front_] = element;

        front_ = (front_ + 1) % size_;
        numElements_++;

        return true;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::placeFront(const T &&element, bool overwrite)
    {

        if (numElements_ == size_)
        {

            if (overwrite)
                removeBack();
            else
                return false;
        }

        bufferArray_[front_] = element;

        front_ = (front_ + 1) % size_;
        numElements_++;

        return true;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::placeBack(const T &element, bool overwrite)
    {

        if (numElements_ == size_)
        {

            if (overwrite)
                removeFront();
            else
                return false;
        }

        if (back_ == 0)
            back_ = size_ - 1;
        else
            back_--;

        bufferArray_[back_] = element;

        numElements_++;

        return true;
    }

    template <typename T, size_t size_>
    inline bool Buffer<T, size_>::placeBack(const T &&element, bool overwrite)
    {

        if (numElements_ == size_)
        {

            if (overwrite)
                removeFront();
            else
                return false;
        }

        if (back_ == 0)
            back_ = size_ - 1;
        else
            back_--;

        bufferArray_[back_] = element;

        numElements_++;

        return true;
    }

} // namespace VCTR

#endif