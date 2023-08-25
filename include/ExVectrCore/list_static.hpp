#ifndef EXVECTRCORE_LISTSTATIC_H
#define EXVECTRCORE_LISTSTATIC_H

#include "stdint.h"
#include "stddef.h"
#include "math.h"

#include "list.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * @brief   Implements a fixed size, statically allocated list. Internally a simple array is used.
         *
         * @tparam  T Type of elements in ListStatic.
         */
        template <typename T, size_t L>
        class ListStatic : public List<T>
        {
        private:

            /**
             * @brief Array
             */
            T items_[L];

        public:
            ListStatic();

            ~ListStatic(); // Remember to remove itsself from linked list

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
             * @returns item at given index.
             */
            T &operator[](size_t index) override;

            /**
             * @returns item at given index.
             */
            const T &operator[](size_t index) const override;

            /**
             * Needs to be overloaded to also copy the data to instance.
             * Not doing this will cause 2 ListStatics to share the exact same elements.
             */
            // ListStatic operator=(const ListStatic &toBeCopied);
        };

        template <typename T, size_t L>
        ListStatic<T, L>::ListStatic()
        {}

        template <typename T, size_t L>
        ListStatic<T, L>::~ListStatic()
        {}

        template <typename T, size_t L>
        const T* ListStatic<T, L>::getPtr() const
        {
            return items_;
        }

        template <typename T, size_t L>
        size_t ListStatic<T, L>::size() const
        {
            return L;
        }

        template <typename T, size_t L>
        T &ListStatic<T, L>::operator[](size_t index)
        {
            return items_[index];
        }

        template <typename T, size_t L>
        const T &ListStatic<T, L>::operator[](size_t index) const
        {
            return items_[index];
        }

        /*template <typename T, size_t L>
        ListStatic<T, L> ListStatic<T, L>::operator=(const ListStatic &toBeCopied)
        {



            return *this;
        }*/

    }
} // namespace VCTR

#endif