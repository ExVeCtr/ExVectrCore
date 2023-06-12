#ifndef EXVECTRCORE_TOPICSUBSCRIBERS_H
#define EXVECTRCORE_TOPICSUBSCRIBERS_H

#include "topic.hpp"
#include "list_buffer.hpp"
#include "list_array.hpp"

#include "stddef.h"

namespace VCTR
{

    namespace Core
    {

        /**
         * This header contains the implementation of a few subscriber classes.
         *
         * Simple_Subscriber is fastest and only contains one item.
         * Buffer_Subscriber is identical to FiFoBuffer but auto adds items to beginning of buffer.
         */

        /**
         * Subscribes to topic(s). Used by other subscribers to implement core functions.
         * void receive(TYPE const& item, const Topic<TYPE>* topic) function must be implemented and will be called when receiving a published item.
         */
        template <typename TYPE>
        class Subscriber_Generic : public Subscriber_Interface<TYPE>
        {
            friend Topic<TYPE>;

        private:
            // List of pointers to all subscribed topics.
            ListArray<Topic<TYPE> *> subscribedTopics_;

        public:
            Subscriber_Generic() {}

            Subscriber_Generic(Topic<TYPE> &topic)
            {
                subscribe(topic);
            }

            virtual ~Subscriber_Generic() {}

            /**
             * Will remove all subscriptions.
             */
            void unsubcribe() override
            {
                for (size_t i = 0; i < subscribedTopics_.size(); i++)
                {
                    subscribedTopics_[i]->removeSubscriber(*this);
                }
                subscribedTopics_.clear();
            }

            /**
             * Will remove subscribtion. Will not receive anymore.
             */
            void unsubcribe(Topic<TYPE> &topic) override
            {
                topic.removeSubscriber(*this);
                subscribedTopics_.removeAllEqual(&topic);
            }

            /**
             * Subscribes to given topic. Will not remove old subscriptions.
             * @param topic Topic to subscribe to.
             */
            void subscribe(Topic<TYPE> &topic) override
            {
                topic.addSubscriber(this);
                subscribedTopics_.removeAllEqual(&topic);
                subscribedTopics_.append(&topic);
            }

            /**
             * Publishes an item to topic, but will not receive item. Makes it simpler to broadcast items from modules.
             * @param item Item to publish
             */
            void publish(TYPE const &item)
            {
                for (size_t i = 0; i < subscribedTopics_.size(); i++)
                    subscribedTopics_[i]->publish(item, this);
            }
        };

        /**
         * This subscriber contains only one item that is updated on every publish.
         * @see Buffer_Subscriber for receiving more them one item.
         * Use isDataNew() to check if an item was updated.
         */
        template <typename TYPE>
        class Simple_Subscriber : public Subscriber_Generic<TYPE>
        {
        private:
            bool itemIsNew = false;

            TYPE receivedItem;

            void receive(TYPE const &item, const Topic<TYPE> *topic) override
            {
                receivedItem = item;
                itemIsNew = true;
            }

        public:
            Simple_Subscriber() {}

            /**
             * @param topic Topic to subscribe to.
             */
            Simple_Subscriber(Topic<TYPE> &topic) : Subscriber_Generic<TYPE>(topic) {}

            /**
             * @returns True if new data was received
             */
            inline bool isDataNew() const { return itemIsNew; }

            /**
             * @returns reference to item.
             */
            inline const TYPE &getItem()
            {
                itemIsNew = false;
                return receivedItem;
            }

        };

        /**
         * This subscriber can only publish data to the subscribed topic. Is more optimised than others and allows safe connections.
         */
        template <typename TYPE>
        class Topic_Publisher : public Subscriber_Generic<TYPE>
        {
        private:

            void receive(TYPE const &item, const Topic<TYPE> *topic) override
            {
                // Do nothing
            }

        public:
            Topic_Publisher() {}

            /**
             * @param topic Topic to subscribe to.
             */
            Topic_Publisher(Topic<TYPE> &topic) : Subscriber_Generic<TYPE>(topic) {}
        };

        /**
         * This subscriber implements a Fifo. New items are placed into Fifo front.
         * @see Simple_Subscriber for receiving only one item.
         *
         */
        template <typename TYPE, size_t SIZE>
        class Buffer_Subscriber : public Subscriber_Generic<TYPE>, public ListBuffer<TYPE, SIZE>
        {
        public:
            Buffer_Subscriber(bool overwrite = false) {}

            /**
             * @param topic Topic to subscribe to.
             * @param overwrite Set to true to overwrite oldest values if full. Defaults to false.
             */
            Buffer_Subscriber(Topic<TYPE> &topic, bool overwrite = false) : Subscriber_Generic<TYPE>(topic)
            {
                overwrite_ = overwrite;
            }

            /**
             * Sets if subscriber should overwrite when fifo is full.
             * @param overwrite Whether to overwrite.
             */
            void setOverwrite(bool overwrite) { overwrite_ = overwrite; }

        private:
            void receive(TYPE const &item, const Topic<TYPE> *topic) override
            {
                this->placeFront(item, overwrite_);
            }

            bool overwrite_ = false;
        };

        /**
         * This subscriber calls the given function passing the data received form topic to it.
         */
        template <typename TYPE, typename CALLBACKTYPE>
        class Callback_Subscriber : public Subscriber_Generic<TYPE>
        {
        public:
            Callback_Subscriber() {}

            /**
             * e.g. Callback_Subscriber<int, class> subscriber(intTopic);
             * @param topic Topic to subscribe to. But there is no object or object to call.
             */
            Callback_Subscriber(Topic<TYPE> &topic) : Subscriber_Generic<TYPE>(topic)
            {
                callbackFunc_ = nullptr;
                object_ = nullptr;
            }

            /**
             * e.g. Callback_Subscriber<int, class> subscriber(intTopic, this, class::intTopicEventCallback);
             * @param topic Topic to subscribe to.
             * @param objectPointer Which object to call the callback function on.
             * @param callbackFunc Function to call on data receive. Returns void and parameter is <TYPE>& item.
             */
            Callback_Subscriber(Topic<TYPE> &topic, CALLBACKTYPE *objectPointer, void (CALLBACKTYPE::*callbackFunc)(const TYPE &)) : Subscriber_Generic<TYPE>(topic)
            {
                callbackFunc_ = callbackFunc;
                object_ = objectPointer;
            }

            /**
             * Sets what object to call the callback function on, on receive.
             * @param objectPointer Which object to call the function on.
             */
            void setCallbackObject(CALLBACKTYPE *objectPointer)
            {
                object_ = objectPointer;
            }

            /**
             * Sets what callback function to be called on receive.
             * @note example: setCallbackFunction(&Foo::callbackFunc);
             * @param callbackFunc Which function to call on receive.
             */
            void setCallbackFunction(void (CALLBACKTYPE::*callbackFunc)(const TYPE &))
            {
                callbackFunc_ = callbackFunc;
            }

            /**
             * @returns callback function to be called on receive. If none then returns nullptr.
             */
            /*void ()(TYPE& item) getCallbackFunction() {
                return callbackFunc_;
            }*/

        private:
            void receive(TYPE const &item, const Topic<TYPE> *topic) override
            {
                if (callbackFunc_ != nullptr && object_ != nullptr)
                    (object_->*callbackFunc_)(item);
            }

            void (CALLBACKTYPE::*callbackFunc_)(const TYPE &) = nullptr;
            CALLBACKTYPE *object_ = nullptr;
        };

        /**
         * This subscriber calls the given function passing the data received form topic to it.
         */
        template <typename TYPE>
        class StaticCallback_Subscriber : public Subscriber_Generic<TYPE>
        {
        public:
            StaticCallback_Subscriber() {}

            /**
             * @param callbackFunc Function to call on data receive.
             */
            StaticCallback_Subscriber(void (*callbackFunc)(TYPE const &item))
            {
                callbackFunc_ = callbackFunc;
            }

            /**
             * @param topic Topic to subscribe to.
             * @param callbackFunc Function to call on data receive.
             */
            StaticCallback_Subscriber(Topic<TYPE> &topic, void (*callbackFunc)(TYPE const &item)) : Subscriber_Generic<TYPE>(topic)
            {
                callbackFunc_ = callbackFunc;
            }

            /**
             * Sets which function to be called on item receive. Give a nullptr to disable.
             * @param callbackFunc Function to be called and given received item
             */
            void setCallbackFunction(void (*callbackFunc)(TYPE const &item))
            {
                callbackFunc_ = callbackFunc;
            }

        private:
            void receive(TYPE const &item, const Topic<TYPE> *topic) override
            {
                if (callbackFunc_ != nullptr)
                    callbackFunc_(item);
            }

            void (*callbackFunc_)(TYPE const &item) = nullptr;
        };

    }

}

#endif