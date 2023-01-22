#ifndef VECTRCORE_UTILITIES_LIST_H
#define VECTRCORE_UTILITIES_LIST_H

#include "stdint.h"

namespace VCTR
{

    /**
     * This is a abstract class for defining the interface for different list data types. Some examples are list array, list fixed, list linked, list ringbuffer.
     * Each different type has its pros and cons and different behaviors.
     * @param TYPE type of data to store in List
     */
    template <typename TYPE>
    class List
    {   
        public:

        virtual ~List() {} //Virtual destructor. As needed due to inheretance of this class.
        
        /**
         * @brief Will return the size of the list. (Aka the length)
        */
        virtual size_t size() const = 0;

        /**
         * @brief Use this to access the items inside the list.
         * @returns a reference to the item at the given index.
        */
        virtual TYPE& operator[] (size_t index) = 0;

        /**
         * @brief Use this to access the items inside the list. Items cannot be modified!
         * @returns a const reference to the item at the given index.
        */
        virtual const TYPE& operator[] (size_t index) const = 0;

        /**
         * @brief   Will copy the items in the given list into the list. The number of items to be copied is the size of the smaller array. 
         *          This is usually the size of the array being copied but can be limited by the copying array if its a fixed size and smaller.
         *          Not required to be implemented by child class but can improve performance by doing so.
         * @returns reference to this array.
        */
        virtual List<TYPE>& operator= (const List<TYPE>& listB);
        
        /**
         * @brief   Will copy the items in the given list into the list. The number of items to be copied is the size of the smaller array. 
         *          As this function is receiving a list with a different data type. The data must be casted and the looping is slower. Are you sure about this?
         * @returns reference to this array.
        */
        template <typename TYPE2>
        List<TYPE>& operator= (const List<TYPE2>& listB);
    };

    template<typename TYPE>
    List<TYPE>& List<TYPE>::operator= (const List<TYPE>& listB) {

        for (size_t i = 0; i < size() && i < listB.size(); i++) (*this)[i] = listB[i];

        return *this;

    }

    template<typename TYPE>
    template<typename TYPE2>
    List<TYPE>& List<TYPE>::operator= (const List<TYPE2>& listB) {

        for (size_t i = 0; i < size() && i < listB.size()) this->operator[i] = static_cast<TYPE>(listB[i]);

        return *this;

    }


}

#endif