#ifndef EXVECTRCORE_TOPIC_H
#define EXVECTRCORE_TOPIC_H

#include "stddef.h"
// #include "list_array.hpp"
#include "list_linked.hpp"
#include "list_static.hpp"

namespace VCTR
{

    namespace Core
    {

        template <typename TYPE>
        class Subscriber;

        template <typename TYPE>
        class Topic final
        {
            friend Subscriber<TYPE>;

        private:
            // List of subscribers.
            // ListArray<Subscriber_Interface<TYPE> *> subscribers_;
            ListLinked<Subscriber<TYPE> *> *subListStart_ = nullptr;

        public:
            Topic() {}

            ~Topic();

            /**
             * @returns list of all subscribers.
             */
            const List<Subscriber<TYPE> *> &getSubscriberList() const;

            /**
             * Sends item to all subscribers.
             * @param item Item to be sent.
             */
            void publish(const TYPE &item);

        private:
            /**
             * Publishes copy of item to subscribers except for given subscriber.
             * @param item Item to be sent.
             * @param subscriber Subscriber to not receive item
             */
            void publish(const TYPE &item, Subscriber<TYPE> *subscriber);

            // TYPE latestItem;
        };

        /**
         * Abstract class used by subscribers to receive topic data.
         * void receive(const TYPE& item, const Topic<TYPE>* topic) function must be implemented and will be called when receiving a published item.
         */
        template <typename TYPE>
        class Subscriber
        {
            friend Topic<TYPE>;

        private:
            // Topic should only give items if this is true.
            bool receiveItems_ = true;
            // Element to topic subscriber list
            ListLinked<Subscriber<TYPE> *> subListElement_;
            // Topic this is subscribed to. Is nullptr if not subscribed.
            Topic<TYPE> *subbedTopic_ = nullptr;

        public:
            Subscriber()
            {
                receiveItems_ = true;
                subListElement_[0] = this;
            }

            virtual ~Subscriber()
            {
                unsubscribe();
            }

            /**
             * Sets the subscriber to receive a item from Topic. Removes need to subscribe and unsubscribe which can be costly.
             * @param isEnabled Set to true to receive items. Defaults to true.
             */
            void receiveEnable(bool isEnabled = true)
            {
                receiveItems_ = isEnabled;
            }

            /**
             * @returns whether or not receiving is enabled.
             */
            bool isReceiveEnabled() { return receiveItems_; }

            /**
             * @brief Subscribes to given topic. Unsubscribes to previous subscriber.
             * @param topic
             */
            void subscribe(Topic<TYPE> &topic);

            /**
             * @brief Unsubscribes. Will not receive any more published items.
             */
            void unsubscribe();

            /**
             * @brief Publishes the given item to subscribed topic, but will not receive its item.
            */
            void publish(const TYPE &item);

        protected:
            /**
             * This is called when subscriber is supposed to receive an item.
             * @param item Item that subscriber will receive.
             * @param topic Which topic is calling this receive function.
             */
            virtual void receive(const TYPE &item, const Topic<TYPE> *topic) = 0;
        };

        template <typename TYPE>
        Topic<TYPE>::~Topic()
        {

            ListLinked<Subscriber<TYPE> *> *next = subListStart_;
            while (next != nullptr && next != subListStart_)
            {

                (*next)[0]->unsubscribe();
                next = next->getNext();
            }
        }

        template <typename TYPE>
        const List<Subscriber<TYPE> *> &Topic<TYPE>::getSubscriberList() const
        {
            if (subListStart_ == nullptr)
            {
                return ListStatic<Subscriber<TYPE> *, 0>();
            }
            return *subListStart_;
        }

        template <typename TYPE>
        void Topic<TYPE>::publish(const TYPE &item)
        {

            ListLinked<Subscriber<TYPE> *> *next = subListStart_;
            if (subListStart_ == nullptr)
                return;

            do { // Welcome to this coding tour. Here we can see a very rare do while loop in a place it makes sense.

                if ((*next)[0]->receiveItems_)
                {
                    (*next)[0]->receive(item, this);
                }
                next = next->getNext();
            } while (next != nullptr && next != subListStart_);
        }

        template <typename TYPE>
        void Topic<TYPE>::publish(const TYPE &item, Subscriber<TYPE> *subscriber)
        {

            ListLinked<Subscriber<TYPE> *> *next = subListStart_;
            if (subListStart_ == nullptr)
                return;

            do { // Welcome to this coding tour. Here we can see a very rare do while loop in a place it makes sense.

                if ((*next)[0]->receiveItems_)
                {
                    (*next)[0]->receive(item, this);
                }
                next = next->getNext();
            } while (next != nullptr && next != subListStart_);
        }

        template <typename TYPE>
        void Subscriber<TYPE>::publish(const TYPE &item)
        {
            if (subbedTopic_ != nullptr)
                subbedTopic_->publish(item, this);
        }

        template <typename TYPE>
        void Subscriber<TYPE>::subscribe(Topic<TYPE> &topic)
        {   

            subbedTopic_ = &topic;

            if (topic.subListStart_ == nullptr)
            {
                topic.subListStart_ = &subListElement_;
                return;
            }

            // Check if the subscriber is already in the list. Leave if it is.
            ListLinked<Subscriber<TYPE> *> *next = topic.subListStart_;
            while (next != nullptr && next != topic.subListStart_)
            {
                if ((*next)[0] == this)
                {
                    return;
                }
            }

            topic.subListStart_->append(subListElement_);

        }

        template <typename TYPE>
        void Subscriber<TYPE>::unsubscribe()
        {

            if (subbedTopic_ == nullptr) //Already not subscribed.
                return; 
            
            if (subbedTopic_->subListStart_ == &subListElement_) //We are start of list. Set topic list begin to next element. If we are last then this will be nullptr.
            {
                subbedTopic_->subListStart_ = subListElement_.getNext();
                subbedTopic_ = nullptr;
                return;
            }

            ListLinked<Subscriber<TYPE> *> *next = subbedTopic_->subListStart_;
            while (next != nullptr && next != subbedTopic_->subListStart_)
            {
                if ((*next)[0] == this)
                    (*next).remove();
            }

            subbedTopic_ = nullptr;

        }

    }

}

#endif