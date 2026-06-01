#ifndef EXVECTRCORE_TOPICROUTERS_H
#define EXVECTRCORE_TOPICROUTERS_H

#include "stddef.h"

#include "list_array.hpp"

#include "topic.hpp"
#include "topic_subscribers.hpp"

namespace VCTR {

namespace Core {

/**
 * This header contains the implementation of a few router classes for routing
 * data between topics.
 */

/**
 * @brief This router simply transfers data from one topic to all other topics.
 * @note This is monodirectional. It only transfers from subscribed topic to
 * forward topics. Use multiple routers to transfer in both directions.
 */
template <typename TYPE> class TopicRouter : public Subscriber_Interface<TYPE> {
private:
  ListArray<Topic<TYPE> *> topicsList_;

  int64_t minimalInterval_ = 0; // Minimal interval between messages. Used to
                                // prevent flooding the bus with messages.
  int64_t lastTime_ = 0;        // Last time a message was sent. Used to prevent
                                // flooding the bus with messages.

public:
  /**
   * @brief Constructor for the TopicRouter class.
   * @param minimalInterval Minimal interval between messages. Used to prevent
   * flooding the bus with messages.
   */
  TopicRouter(int64_t minimalInterval = 0)
      : minimalInterval_(minimalInterval) {}

  void setInterval(int64_t interval) { minimalInterval_ = interval; }

  int64_t getInterval() { return minimalInterval_; }

  /**
   * @returns True if new data was received
   */
  void addTopicToForward(Topic<TYPE> &topicToForward);

private:
  /**
   * Will recieve a message from topic and forward it to all other topics in
   * list.
   * @param item Item to forward to other topic.
   * @param topic Which toic called this receive() function that will not get
   * item forwarded to.
   */
  void receive(TYPE &item, const Topic<TYPE> *topic) override;
};

template <typename TYPE>
void TopicRouter<TYPE>::addTopicToForward(Topic<TYPE> &topicToForward) {
  topicsList_.appendIfNotInList(&topicToForward);
}

template <typename TYPE>
void TopicRouter<TYPE>::receive(TYPE &item, const Topic<TYPE> *topic) {

  // Check if the time since last message is greater than the minimal interval.
  // If not, do not forward the message.
  if ((Core::Now() - lastTime_) < minimalInterval_) {
    return;
  }

  lastTime_ = Core::Now();

  for (size_t i = 0; i < topicsList_.size(); i++) {
    Topic<TYPE> *topicForward = topicsList_[i];
    if (topicForward != &topic)
      topicForward->publish(item, this);
  }
}

} // namespace Core

} // namespace VCTR

#endif