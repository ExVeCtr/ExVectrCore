#ifndef EXVECTRCORE_LISTEXTERN_HPP
#define EXVECTRCORE_LISTEXTERN_HPP

#include "stdint.h"
#include "stddef.h"
#include "math.h"

#include "list.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * @brief   Implements a list using an external array or datapointer as storage.
         *
         * @tparam  T Type of elements in ListStatic.
         */
        template <typename T>
        class ListExtern : public List<T>
        {
        private:

            /**
             * @brief Array
             */
            T* items_ = nullptr;

            /**
             * @brief Size of array
             */
            size_t size_ = 0;

        public:
            ListExtern();

            /**
             * @brief Ctor what also sets the pointer and size.
             * @param ptr 
             * @param size 
             */
            ListExtern(T* ptr, size_t size);

            ~ListExtern(); // Remember to remove itsself from linked list

            /**
             * @note 
             * @returns number of elements in list.
             */
            size_t size() const override;
            
            /**
             * @returns pointer to first element in list.
             */
            const T* getPtr() const;

            /**
             * @brief Sets the pointer to the given pointer and size.
             * @param ptr 
             * @param size 
             */
            void setPtr(T* ptr, size_t size);

            /**
             * @returns item at given index.
             */
            T &operator[](size_t index) override;

            /**
             * @returns item at given index.
             */
            const T &operator[](size_t index) const override;
        };

        template <typename T>
        ListExtern<T>::ListExtern()
        {}

        template <typename T>
        ListExtern<T>::ListExtern(T* ptr, size_t size)
        {
            setPtr(ptr, size);
        }

        template <typename T>
        ListExtern<T>::~ListExtern()
        {}

        template <typename T>
        const T* ListExtern<T>::getPtr() const
        {
            return items_;
        }

        template <typename T>
        size_t ListExtern<T>::size() const
        {
            return size_;
        }

        template <typename T>
        void ListExtern<T>::setPtr(T* ptr, size_t size)
        {
            items_ = ptr;
            size_ = size;
        }

        template <typename T>
        T &ListExtern<T>::operator[](size_t index)
        {
            return items_[index];
        }

        template <typename T>
        const T &ListExtern<T>::operator[](size_t index) const
        {
            return items_[index];
        }

    }
} // namespace VCTR

#endif