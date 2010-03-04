/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Universities Space Research Association nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * MessageQueueMap.cpp
 *
 *  Created on: Feb 19, 2010
 *      Author: jhogins
 */

#include "MessageQueueMap.h"
#include <map>
#include <utility>

namespace PLEXIL {

MessageQueueMap::MessageQueueMap(AdapterExecInterface& execInterface, bool allowDuplicateMessages) :
  m_mutex(),
  m_execInterface(execInterface),
  m_allowDuplicateMessages(allowDuplicateMessages) {
}

MessageQueueMap::~MessageQueueMap() {
}

/**
 * @brief Adds the given recipient to the queue to receive the given message.
 * If a recipient already exists for this message, messages will be handed out in the order
 * of the adding of the recipients.
 * @param message The message the recipient is waiting for
 * @param dest The command destination
 * @param ack The command acknowledgment
 */
void MessageQueueMap::addRecipient(const LabelStr& message, const ExpressionId& ack, const ExpressionId& dest) {
  m_mutex.lock();
  PairingQueue* que = getQueue(message);
  que->m_recipientQueue.push_back(Recipient(ack, dest));
  updateQueue(que);
  debugMsg("MessageQueueMap:addRecipient", "Recipient for message \"" << que->m_name << "\" added");
  m_mutex.unlock();
}
/**
 * @brief Removes all instances of the given recipient waiting on the given message string.
 * @return True if a recipient was removed, false otherwise
 */
bool MessageQueueMap::removeRecipient(const LabelStr& message, const ExpressionId ack) {
  m_mutex.lock();
  PairingQueue* pq = getQueue(message);
  for (RecipientQueue::iterator it = pq->m_recipientQueue.begin(); it != pq->m_recipientQueue.end(); it++) {
    if (it->m_ack.equals(ack)) {
      it = pq->m_recipientQueue.erase(it);
      //this increments the iterator, so check for the end immediately
      if (it == pq->m_recipientQueue.end())
        break;
    }
  }
  m_mutex.unlock();
}
/**
 * @brief Removes all recipients waiting on the given message string.
 * @return True if at least one recipient was removed, false otherwise.
 */
bool MessageQueueMap::clearRecipientsForMessage(const LabelStr& message) {
  m_mutex.lock();
  PairingQueue* pq = getQueue(message);
  pq->m_recipientQueue.clear();
  m_mutex.unlock();
}
/**
 * @brief Adds the given message to its queue. If there is a recipient waiting for the message, it is sent immediately.
 * @param message The message string to be added
 */
void MessageQueueMap::addMessage(const LabelStr& message) {
  m_mutex.lock();
  PairingQueue* pq = getQueue(message);
  if (!pq->m_allowDuplicateMessages)
    pq->m_messageQueue.clear();
  pq->m_messageQueue.push_back(StoredArray(1, message.getKey()));
  updateQueue( pq);
  debugMsg("MessageQueueMap:addMessage", "Message \"" << pq->m_name << "\" added");
  m_mutex.unlock();
}
/**
 * @brief Adds the given message with the given parameters to its queue.
 * If there is a recipient waiting for the message, it is sent immediately.
 * @param message The message string to be added
 * @param params The parameters that are to be sent with the message
 */
void MessageQueueMap::addMessage(const LabelStr& message, const PLEXIL::StoredArray& params) {
  m_mutex.lock();
  PairingQueue* pq = getQueue(message);
  if (!pq->m_allowDuplicateMessages)
    pq->m_messageQueue.clear();
  pq->m_messageQueue.push_back(params);
  updateQueue( pq);
  debugMsg("MessageQueueMap:addMessage", "Message \"" << pq->m_name << "\" added");
  m_mutex.unlock();
}

/**
 * @brief Sets the flag that determines whether or not incoming messages
 * with duplicate strings are queued. If true, all incoming messages are
 * put into the queue. Oldest instances of the message are distributed first.
 * If false, new messages with duplicate strings replace older ones; this
 * will remove all oldest duplicates from the queue immediately as well
 * as set the behavior for future messages.
 * @param flag If false, duplicates will be replaced with the newest
 * message. If true, duplicates are queued.
 */
void MessageQueueMap::setAllowDuplicateMessages(bool flag) {
  m_mutex.lock();
  for (std::map<LabelStr, PairingQueue*>::iterator it = m_map.begin(); it != m_map.end(); it++) {
    MessageQueue mq = it->second->m_messageQueue;
    //if setting flag from true to false, ensure all queues have at most one message
    if (m_allowDuplicateMessages && !flag && mq.size() > 1) {
      MessageQueue::iterator begin = mq.begin();
      MessageQueue::iterator end = mq.end();
      end-= 2;
      mq.erase(begin, end);
    }
    it->second->m_allowDuplicateMessages = flag;
  }
  m_allowDuplicateMessages = flag;
  m_mutex.unlock();
}

MessageQueueMap::PairingQueue * MessageQueueMap::getQueue(const LabelStr& message) {
  PairingQueue* result;
  std::map<LabelStr, PairingQueue*>::iterator it = m_map.find(message);
  if (m_map.end() == it) {
    result = new PairingQueue(message, m_allowDuplicateMessages);
    m_map.insert(it, std::pair<LabelStr, PairingQueue*> (message, result));
  } else {
    result = it->second;
  }
  return result;
}
/**
 * @brief Resolves matches between messages and recipients. Should be called whenever updates occur to a queue.
 */
void MessageQueueMap::updateQueue(PairingQueue* queue) {
  MessageQueue& mq = queue->m_messageQueue;
  RecipientQueue& rq = queue->m_recipientQueue;
  MessageQueue::iterator mqIter = mq.begin();
  RecipientQueue::iterator rqIter = rq.begin();
  bool valChanged = !mq.empty() && !rq.empty();
  while (! (mqIter == mq.end()) && !(rqIter == rq.end())) {
    if ((*mqIter).size() == 1) {
      m_execInterface.handleValueChange(rqIter->m_dest, (*mqIter).at(0));
    } else {
      m_execInterface.handleValueChange(rqIter->m_dest, (*mqIter).getKey());
    }
    rqIter = rq.erase(rqIter);
    mqIter = mq.erase(mqIter);
  }
  if (valChanged) {
    debugMsg("MessageQueueMap:updateQueue", "Message \"" << queue->m_name << "\" paired and sent");
    m_execInterface.notifyOfExternalEvent();
  }
}
}