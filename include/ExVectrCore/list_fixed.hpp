#ifndef EXVECTRCORE_LISTFIXED_H
#define EXVECTRCORE_LISTFIXED_H

#include "stddef.h"

#include "list.hpp"

namespace VCTR
{

    /**
     * This is a list for ordinary fixed size arrays. This is the absolute fastest list type but the size is fixed on compilation. The data is stored on stack memory. 
     * @param TYPE type of data to store in ListFixed
     */
    template <typename TYPE, size_t SIZE>
    class ListFixed : public List<TYPE>
    {
    private:

        TYPE[SIZE] array_;

    public:
        
        ListFixed() {}

        inline size_t size() const;

        /**
         * @param index Index of item to be returned.
         * @returns reference to item in ListFixed
         */
        TYPE &operator[](size_t index);

        /**
         * @param index Index of item to be returned.
         * @returns reference to item in ListFixed
         */
        const TYPE &operator[](size_t index) const;

        /**
         * @brief   Will copy the items in the given list into the list. The number of items to be copied is the size of the smaller array.
         * @returns reference to this array.
        */
        template <typename TYPE2>
        List<TYPE> &operator=(const List<TYPE2> &listB)

    };

    template <typename TYPE, size_t SIZE>
    inline size_t ListFixed<TYPE, SIZE>::size() const
    {
        return SIZE;
    }

    template <typename TYPE, size_t SIZE>
    TYPE &ListFixed<TYPE, SIZE>::operator[](size_t index)
    {
        return array_[index];
    }

    template <typename TYPE, size_t SIZE>
    const TYPE &ListFixed<TYPE, SIZE>::operator[](size_t index) const
    {
        return array_[index];
    }

    template <typename TYPE, size_t SIZE>
    template <typename TYPE2>
    List<TYPE> &ListFixed<TYPE, SIZE>::operator=(const List<TYPE2> &listB)
    {

        for (size_t i = 0; i < SIZE && i < listB.size()) array[i] = listB[i];

        return *this;
    }

}

#endif