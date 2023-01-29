#ifndef EXVECTRCORE_TOPICROUTERS_H
#define EXVECTRCORE_TOPICROUTERS_H

#include "list_array.hpp"

#include "topic.hpp"
#include "topic_subscribers.hpp"

namespace VCTR
{

    /**
     * This header contains the implementation of a few router classes for routing data between topics.
     */

    /**
     * This router simply transfers data from one topic to all other topics.
     */
    template <typename TYPE>
    class TopicRouter : public Subscriber_Interface<TYPE>
    {
    private:
        ListArray<Topic<TYPE> *> topicsList_;

    public:
        TopicRouter() {}

        /**
         * @returns True if new data was received
         */
        void addTopicToForward(Topic<TYPE> &topicToForward);

    private:
        /**
         * Will recieve a message from topic and forward it to all other topics in list.
         * @param item Item to forward to other topic.
         * @param topic Which toic called this receive() function that will not get item forwarded to.
         */
        void receive(TYPE &item, const Topic<TYPE> *topic) override;
    };

    template <typename TYPE>
    void TopicRouter<TYPE>::addTopicToForward(Topic<TYPE> &topicToForward)
    {
        topicsList_.appendIfNotInList(&topicToForward);
    }

    template <typename TYPE>
    void TopicRouter<TYPE>::receive(TYPE &item, const Topic<TYPE> *topic)
    {
        for (uint32_t i = 0; i < topicsList_.size(); i++)
        {
            Topic<TYPE> *topicForward = topicsList_[i];
            if (topicForward != &topic)
                topicForward->publish(item, this);
        }
    }

}

#endif