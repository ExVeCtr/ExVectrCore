#ifndef EXVECTRCORE_LISTLINKED_H
#define EXVECTRCORE_LISTLINKED_H

#include "stdint.h"
#include "stddef.h"
#include "math.h"

#include "list.hpp"

namespace VCTR
{

    namespace Core
    {

        /**
         * @brief   ListLinked class. Inherits from List. Is an element in a linked list. Multiple ListLinkeds can be linked together to form a linked list.
         * @note    ListLinked is not a linked list. It is an element in a linked list, therefore all index operations are relative to the ListLinked it is called on. Index 0 is the ListLinked it is called on.
         *
         * @tparam  T Type of elements in ListLinked.
         */
        template <typename T>
        class ListLinked : public List<T>
        {
        private:
            /**
             * @brief Pointer to next ListLinked in linked list.
             */
            ListLinked *next_ = nullptr;

            /**
             * @brief Pointer to previous ListLinked in linked list.
             */
            ListLinked *prev_ = nullptr;

            /**
             * @brief Item that is stored in ListLinked.
             */
            T item_;

        public:
            ListLinked();

            ~ListLinked(); // Remember to remove itsself from linked list

            /**
             * @note Expensive operation. O(n) time complexity.
             * @returns number of elements in list.
             */
            size_t size() const override;

            /**
             * @note Expensive operation. O(n) time complexity and will return the object it is called on if list is circular.
             * @returns the start of the list.
             */
            ListLinked *getBegin() const;

            /**
             * @note Expensive operation. O(n) time complexity and will return the object it is called on if list is circular.
             * @returns the end of the list.
             */
            ListLinked *getEnd() const;

            /**
             * @note Cheap operation. O(1) time complexity.
             * @returns the start of the list. Nullptr if at end of list.
             */
            ListLinked *getNext() const;

            /**
             * @note Cheap operation. O(1) time complexity.
             * @returns the start of the list. Nullptr if at start of list.
             */
            ListLinked *getPrev() const;

            /**
             * @brief Inserts item into given index of this list.
             * @param listElement ListLinked to insert.
             * @param index Index to append listElement at.
             */
            void insert(ListLinked &listElement, size_t index);

            /**
             * @brief Appends ListLinked to end of list.
             * @param listElement ListLinked to append.
             */
            void append(ListLinked &listElement);

            /**
             * @brief Removes the item at given index.
             * @note Cheap operation. O(1) time complexity.
             * @param index Default is 0 being the ListLinked it is called on, removing the element it is called on.
             */
            void remove(size_t index = 0);

            /**
             * @brief Gets the ListLinked at given index. Used to access the linked list like an array (index 0 is the ListLinked it is called on).
             * @param index ListLinked to insert.
             */
            ListLinked &getListElement(size_t index = 0);

            /**
             * @brief Splits the list at given index.
             * @param index Index to split list at.
             * @return Beginning of list split off this list.
             */
            // ListLinked* split(size_t index);

            /**
             * @returns item at given index after this ListLinked.
             */
            T &operator[](size_t index) override;

            /**
             * @returns item at given index after this ListLinked.
             */
            const T &operator[](size_t index) const override;

            /**
             * Needs to be overloaded to also copy the data to instance.
             * Not doing this will cause 2 ListLinkeds to share the exact same elements.
             */
            // ListLinked operator=(const ListLinked &toBeCopied);
        };

        template <typename T>
        ListLinked<T>::ListLinked()
        {
        }

        template <typename T>
        ListLinked<T>::~ListLinked()
        {

            if (next_ != nullptr)
            {
                next_->prev_ = prev_;
            }

            if (prev_ != nullptr)
            {
                prev_->next_ = next_;
            }
        }

        template <typename T>
        size_t ListLinked<T>::size() const
        {

            size_t size = 1;
            ListLinked *next = next_;
            while (next != nullptr && next != this) // Go through all elements in list until next is nullptr or in case of circular list, until next is this
            {
                size++;
                next = next->next_;
            }

            return size;
        }

        template <typename T>
        ListLinked<T> *ListLinked<T>::getBegin() const
        {

            ListLinked *begin = this;
            while (begin->prev_ != nullptr && begin->prev_ != this)
            {
                begin = begin->prev_;
            }

            return begin;
        }

        template <typename T>
        ListLinked<T> *ListLinked<T>::getEnd() const
        {

            ListLinked *end = this;
            while (end->next_ != nullptr && end->next_ != this)
            {
                end = end->next_;
            }

            return end;
        }

        template <typename T>
        ListLinked<T> *ListLinked<T>::getNext() const
        {
            return next_;
        }

        template <typename T>
        ListLinked<T> *ListLinked<T>::getPrev() const
        {
            return prev_;
        }

        template <typename T>
        void ListLinked<T>::insert(ListLinked &listElement, size_t index)
        {

            listElement.remove(); // Remove from old list

            ListLinked *next = this;
            ListLinked *prev = prev_;
            for (size_t i = 0; i < index && next_ != nullptr; i++)
            {

                prev = next;
                next = next->next_;
            }

            if (prev != nullptr)
            {
                prev->next_ = &listElement;
            }

            if (next != nullptr)
            {
                next->prev_ = &listElement;
            }

            listElement.next_ = next;
            listElement.prev_ = prev;
        }

        template <typename T>
        void ListLinked<T>::append(ListLinked &listElement)
        {

            listElement.remove(); // Remove from old list

            ListLinked *next = next_;
            ListLinked *prev = this;
            while (next_ != nullptr && next_ != this)
            {

                prev = next;
                next = next->next_;
            }

            prev->next_ = &listElement;

            listElement.next_ = next;
            listElement.prev_ = prev;
        }

        template <typename T>
        void ListLinked<T>::remove(size_t index)
        {

            ListLinked *next = this;
            ListLinked *prev = prev_;
            for (size_t i = 0; i < index && next_->next_ != nullptr; i++)
            {

                prev = next;
                next = next->next_;
            }

            if (prev != nullptr)
            {
                prev->next_ = next->next_;
            }

            next->prev_ = nullptr;
            next->next_ = nullptr;

            if (next->next_ != nullptr)
            {
                next->next_->prev_ = prev;
            }
        }

        template <typename T>
        ListLinked<T> &ListLinked<T>::getListElement(size_t index)
        {

            ListLinked *next = this;
            for (size_t i = 0; i < index && next_->next_ != nullptr; i++)
            {
                next = next->next_;
            }

            return *next;
        }

        template <typename T>
        T &ListLinked<T>::operator[](size_t index)
        {
            return getListElement(index).data_;
        }

        template <typename T>
        const T &ListLinked<T>::operator[](size_t index) const
        {
            return getListElement(index).data_;
        }

        /*template <typename T>
        ListLinked<T> ListLinked<T>::operator=(const ListLinked &toBeCopied)
        {



            return *this;
        }*/

    }
} // namespace VCTR

#endif