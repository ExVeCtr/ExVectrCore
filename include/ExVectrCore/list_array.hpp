#ifndef EXVECTRCORE_LISTARRAY_H
#define EXVECTRCORE_LISTARRAY_H

#include "stddef.h"

#include "list.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * This is a dynamic array. If full, the size will be doubled. Uses heap memory and can be slower. Iterating through list is fast and item access is O(1). Removing or adding an item is O(n).
         * The next item index is always +1. So the same as a normal array.
         * The sizeControl setting on construction can cause heap fragmentation if set to true but will reduce size if unneeded
         * @param TYPE type of data to store in ListArray
         */
        template <typename TYPE>
        class ListArray : public List<TYPE>
        {
        private:
            // Current max size.
            size_t maxSize_ = 0;

            // Current size
            size_t size_ = 0;

            // Pointer to start of array.
            TYPE *array_ = nullptr;

            // Wether to reduce array size automatically.
            bool sizeControl_ = false;

        public:
            /**
             * @param sizeControl if set to true then the ListArray will automatically reduce the internal size to save space. Setting this to false will reduce heap fragmentation. Defaults to false.
             */
            ListArray(bool sizeControl = false)
            {
                sizeControl_ = sizeControl;
                array_ = new TYPE[1];
                maxSize_ = 1;
            }

            ~ListArray()
            {
                if (array_ != nullptr)
                {
                    delete[] array_;
                }
            }

            inline size_t size() const;

            /**
             * @returns current internal array size.
             */
            inline size_t getInternalArrayLength() const;

            /**
             * Adds a copy of the given item to the ListArray.
             * @param item Item to add to ListArray.
             */
            void append(const TYPE &item);

            /**
             * Adds a copy of the given item to the ListArray only if there is no other equal item already in the ListArray.
             * @param item Item to add to ListArray.
             * @returns true if no item found and new item placed into ListArray.
             */
            bool appendIfNotInListArray(const TYPE &item);

            /**
             * @brief Places item into given index. All other items behind given index will be pushed down. If Index is larger than ListArray, then new items will NOT be instantiated.
             *
             * @param item Item to be placed at index
             * @param index Index at which to place item
             */
            void insert(const TYPE &item, size_t index);

            /**
             * Inserts copy of item into ListArray behind infront of the first larger item.
             * @param item Item to add to ListArray.
             */
            // void sortedAppend(const TYPE& item);

            /**
             * Removes the item the pointer points to.
             * @param item Pointer to exact item to be removed.
             * @returns true if found and removed. False if not.
             */
            bool removeWithPointer(const TYPE *item);

            /**
             * Removes the item at the given index.
             * @param index Index of item to be removed
             * @returns true if found and removed. False if not.
             */
            bool removeAtIndex(size_t index);

            /**
             * Removes all items in ListArray equal to given item
             * @param index Index of item to be removed
             * @returns true if found and removed. False if not.
             */
            bool removeAllEqual(const TYPE &item);

            /**
             * Removes all items in ListArray.
             */
            void clear();

            /**
             * @param index Index of itm to be returned.
             * @returns reference to item in ListArray
             */
            TYPE &operator[](size_t index);

            /**
             * @param index Index of itm to be returned.
             * @returns reference to item in ListArray
             */
            const TYPE &operator[](size_t index) const;

            /**
             * @brief   Will copy the items in the given list into the list. The number of items to be copied is the size of the array being copied.
             * @returns reference to this array.
             */
            List<TYPE> &operator=(const List<TYPE> &listB);

            /**
             * This will cut down the size of the array if it is less than half full. Can cause heap fragmentation.
             */
            void reduceSize();

        private:
            /**
             * Changes size to given parameter.
             * Copies items to new array.
             * @param size Size to change to.
             */
            void changeSizeTo(size_t size);
        };

        template <typename TYPE>
        void ListArray<TYPE>::reduceSize()
        {

            if (size_ >= maxSize_ / 2)
                return;

            size_t newSize = size_;

            while (size_ < maxSize_ / 2)
                newSize /= 2;

            changeSizeTo(newSize);
        }

        template <typename TYPE>
        void ListArray<TYPE>::changeSizeTo(size_t size)
        {

            // Leave if already same size
            if (size == maxSize_)
                return;

            // Create new array
            TYPE *newPointer = new TYPE[size];

            // Move contents to new array
            size_t i;
            for (i = 0; i < size_ && i < size; i++)
            {
                newPointer[i] = array_[i];
            }

            // Unallocate old array
            delete[] array_;

            // Switch to new array and params
            array_ = newPointer;
            maxSize_ = size;
            size_ = i;
        }

        template <typename TYPE>
        inline size_t ListArray<TYPE>::size() const
        {
            return size_;
        }

        template <typename TYPE>
        inline size_t ListArray<TYPE>::getInternalArrayLength() const
        {
            return maxSize_;
        }

        template <typename TYPE>
        void ListArray<TYPE>::append(const TYPE &item)
        {

            // Double size if array is too small
            if (maxSize_ == 0)
                changeSizeTo(1);
            else if (size_ >= maxSize_)
                changeSizeTo(maxSize_ * 2);

            array_[size_] = item;

            size_++;
        }

        template <typename TYPE>
        bool ListArray<TYPE>::appendIfNotInListArray(const TYPE &item)
        {

            // Check if item is inside ListArray
            for (size_t i = 0; i < size_; i++)
            {
                if (array_[i] == item)
                    return false;
            }

            append(item);

            return true;
        }

        template <typename TYPE>
        void ListArray<TYPE>::insert(const TYPE &item, size_t index)
        {

            // Make sure the ListArray has a size
            if (maxSize_ == 0)
                changeSizeTo(1);

            if (index >= maxSize_)
            {

                while (index >= maxSize_)
                    changeSizeTo(maxSize_ * 2); // Inefficient but works. Should be changed to calculate the next largest exponent
                size_ = index;
                append(item);
            }
            else
            {

                if (size_ >= maxSize_)
                    changeSizeTo(maxSize_ * 2);

                for (size_t i = size_; i > index; i--)
                    array_[i] = array_[i - 1];

                array_[index] = item;

                size_++;
            }
        }

        template <typename TYPE>
        bool ListArray<TYPE>::removeAtIndex(size_t index)
        {

            if (index >= size_)
                return false;

            for (size_t i = index; i < size_ - 1; i++)
            {
                array_[i] = array_[i + 1];
            }

            size_--;

            if (sizeControl_ && size_ <= maxSize_ / 2)
                changeSizeTo(maxSize_ / 2);

            return true;
        }

        template <typename TYPE>
        bool ListArray<TYPE>::removeWithPointer(const TYPE *item)
        {

            // Find index of item
            for (size_t i = 0; i < size_; i++)
            {

                if (&(array_[i]) == item)
                {

                    removeAtIndex(i);
                    return true;
                }
            }

            // Was not found, return false.
            return false;
        }

        template <typename TYPE>
        bool ListArray<TYPE>::removeAllEqual(const TYPE &item)
        {

            bool foundOne = false;

            // Find index of item
            for (size_t i = 0; i < size_; i++)
            {

                if (array_[i] == item)
                {

                    removeAtIndex(i);
                    foundOne = true;
                }
            }

            return foundOne;
        }

        template <typename TYPE>
        void ListArray<TYPE>::clear()
        {

            if (sizeControl_)
                changeSizeTo(0);
            else
                size_ = 0;
        }

        template <typename TYPE>
        TYPE &ListArray<TYPE>::operator[](size_t index)
        {
            return array_[index];
        }

        template <typename TYPE>
        const TYPE &ListArray<TYPE>::operator[](size_t index) const
        {
            return array_[index];
        }

        template <typename TYPE>
        List<TYPE> &ListArray<TYPE>::operator=(const List<TYPE> &listB)
        {

            clear();

            // Copy items into ListArray
            for (size_t i = 0; i < listB.size(); i++)
                append(listB[i]);

            return *this;
        }

    }

}

#endif