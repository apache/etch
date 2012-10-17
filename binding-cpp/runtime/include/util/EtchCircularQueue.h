/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ETCHCIRCULARQUEUE_H__
#define __ETCHCIRCULARQUEUE_H__

#include "capu/os/Time.h"
#include "capu/os/Mutex.h"
#include "capu/os/CondVar.h"
#include "common/EtchObject.h"
#include "support/EtchMailbox.h"
#include "transport/EtchWho.h"
#include "transport/EtchMessage.h"

/**
 * A circular queue of a fixed size. Elements are added to one
 * end and removed from the other, with the queue size ranging
 * from empty to full. Operations can optionally wait until
 * finished or return status indicating success or failure.
 * For instance, adding to a full queue can wait until an item
 * is removed before adding the new item or it can wait only
 * a specified amount of time before completing successfully
 * or giving up.
 */
class EtchCircularQueue {
public:
  /**
   * Constructs the CircularQueue.
   *
   * @param size the maximum number of items allowed in the queue.
   */
  EtchCircularQueue(capu::uint32_t size);

  /**
   * Constructs the CircularQueue with the maximum number of items
   * defaulted to 10.
   */
  EtchCircularQueue();

  /**
   * Destructor
   */
  virtual ~EtchCircularQueue();

  /**
   * @return the maximum number of items that may be put in the queue.
   */
  capu::uint32_t getSize();

  /////////////////////
  // BASIC QUEUE OPS //
  /////////////////////

  /**
   * @return the current number of items in the queue.
   */
  capu::uint32_t getCount();

  /**
   * @return true if the queue is empty.
   */
  capu::bool_t isEmpty();

  /**
   * @return true if the queue is full.
   */
  capu::bool_t isFull();

  //////////////////////
  // PUBLIC QUEUE OPS //
  //////////////////////

  /**
   * Gets the next available item from the queue, waiting
   * until an item is available or the queue is closed.
   *
   * @param element an item from the queue or null if the queue
   * is closed
   * @return ETCH_OK if the item could be dequeued, ETCH_EINVAL if
   * the param is invalid or ETCH_ERROR if the queue is closed.
   */
   status_t get(EtchMailbox::EtchElement** element);

  /**
   * Gets the next available item from the queue, waiting
   * until an item is available or the queue is closed.
   *
   * @param element an item from the queue or null if maxDelay
   * has been exceeded or the queue is closed.
   * @param maxDelay the maximum time in ms to wait for
   * something to be put in the queue; 0 means wait forever,
   * less than 0 means don't wait at all.
   *
   * @return ETCH_OK if the item could be dequeued, ETCH_EINVAL if
   * the param is invalid, ETCH_TIMEOUT if maxDelay has been exceeded
   * or ETCH_ERROR if an error occurred
   */
   status_t get(EtchMailbox::EtchElement** element, capu::int32_t maxDelay);

  /**
   * Puts an item in the queue, waiting until space is available
   * or the queue is closed.
   *
   * @param obj a non-null item to put in the queue.
   *
   * @return ETCH_OK if the item was placed in the queue, ETCH_EINVAL if
   * the param is invalid or ETCH_ERROR if the queue is closed.
   */
  status_t put(EtchMailbox::EtchElement* obj);

  /**
   * Puts an item in the queue, waiting until space is available
   * or the queue is closed.
   *
   * @param obj a non-null item to put in the queue.
   *
   * @param maxDelay the maximum time in ms to wait for
   * available space the queue; 0 means wait forever,
   * less than 0 means don't wait at all.
   *
   * @return ETCH_OK if the item was placed in the queue,
   *  ETCH_TIMEOUT if maxDelay has been exceeded or ETCH_ERROR if an error occurred
   */
  status_t put(EtchMailbox::EtchElement* obj, capu::int32_t maxDelay);

  /**
   * Closes the queue so that no more items may be put into it.
   * Get will return null when there are no more items to return.
   */
  void close();

  /**
   * @return true if the queue is closed.
   */
  capu::bool_t isClosed();

private:

  /**
   * Gets the item at the head of the queue. Additionally, wakes
   * up the next one waiting for the queue, either to get or put.
   * @param element the item at the head of the queue
   *
   * @return ETCH_OK if the item could be dequeued, ETCH_EINVAL if the param is invalid
   * or ETCH_ERROR if an error occurred
   */
  status_t getAndNotify(EtchMailbox::EtchElement** element);

  /**
   * Puts the item at the tail of the queue. Additionally, wakes
   * up the next one waiting for the queue, either to get or put.
   * @param obj non-null item to put.
   *
   * @return ETCH_OK if the item was placed in the queue, ETCH_EINVAL if
   * the param is invalid or ETCH_ERROR if the queue is closed.
   */
  status_t putAndNotify(EtchMailbox::EtchElement* obj);

  /**
   * Gets the item at the head of the queue.
   * @param element the item at the head of the queue.
   *
   * @return ETCH_OK if the item could be dequeued, ETCH_EINVAL if the param is invalid
   * or ETCH_ERROR if an error occurred
   */
   status_t get0(EtchMailbox::EtchElement** element);

  /**
   * Puts the item at the tail of the queue.
   * @param obj non-null item to put.
   *
   * @return ETCH_OK if the item was placed in the queue, ETCH_EINVAL if
   * the param is invalid or ETCH_ERROR if the queue is closed.
   */
  status_t put0(EtchMailbox::EtchElement* obj);

  EtchMailbox::EtchElement** mItems;
  capu::CondVar mCondVar;
  capu::Mutex mMutex;
  capu::bool_t mClosed;
  capu::uint32_t mCount;
  capu::uint32_t mSize;
  capu::uint32_t mHead;
  capu::uint32_t mTail;
};
#endif	/* ETCHCIRCULARQUEUE_H */
