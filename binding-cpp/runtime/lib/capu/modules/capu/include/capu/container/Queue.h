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

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "capu/Error.h"
#include "capu/Config.h"

namespace capu {

  template <typename T>
  class Queue {

    class QueueNode
    {
    public:
      ~QueueNode()
      {
      }

      QueueNode() {
        mNext = NULL;
      }

      QueueNode(T data) {
        mNext = NULL;
        this->mData = data;
      }

      T mData;
      QueueNode* mNext;
    };

    QueueNode* mCurrent;
    QueueNode* mTail;
    int_t mSize;

  public:

    /**
     * Default Constructor
     */
    Queue();

    /**
     * Destructor
     */
    virtual ~Queue();

    /**
     * Add element to the end of list
     * @param element element that will be added
     * @return CAPU_ENO_MEMORY if allocation of element is failed
     *         CAPU_OK if the element is successfully added
     */
    status_t add(const T &element);


    /**
     * Shifts the iterator to the next position and returns the element if next != NULL
     * @param element
     * @return ETCH_ERANGE if the next of current node that is pointed, is null
     *         CAPU_OK if the next element has been gotten
     *
     */
    status_t next(T* element = NULL);

    /**
     * Check if iterator has next element.
     * @return false if the next of current node that is pointed, is null otherwise true
     */
    bool_t hasNext();

    /**
     * return size of the queue
     * @return return the size of list
     */
    int_t size();

    /**
     * check the queue is empty or not
     * @return true if empty
     *         false otherwise
     */
    bool_t isEmpty();

    /**
     * removes all elements from queue
     *
     * @return CAPU_OK if all of the elements are deleted
     *
     */
    status_t clear();
  };

  template <typename T>
  Queue<T>::Queue()
   : mCurrent(NULL), mTail(NULL), mSize(0) {
    //init queue
  }

  template <typename T>
  Queue<T>::~Queue() {
    clear();
  }

  template <typename T>
  status_t Queue<T>::next(T* element) {
    if (mCurrent->mData == NULL) {
      return CAPU_ERANGE;
    }

    T tmp = mCurrent->mData;
    QueueNode* del = mCurrent;
    mCurrent = mCurrent->mNext;
    delete del;
    --mSize;

    if (element != NULL) {    
      *element = tmp;
    }
    return CAPU_OK;
  }


  template <typename T>
  bool_t Queue<T>::hasNext() {
    return mCurrent != NULL;
  }

  template <typename T>
  int_t Queue<T>::size() {
    return mSize;
  }

  template <typename T>
  status_t Queue<T>::clear() {
    //on the deallocation delete all elements in the queue
    while (mCurrent != NULL) {
      Queue::QueueNode* tmp = mCurrent;
      mCurrent = mCurrent->mNext;
      delete tmp;
    }
    mSize = 0;
    return CAPU_OK;
  }


  //add elements to the end of queue
  template <typename T>
  status_t Queue<T>::add(const T &element) {
    Queue::QueueNode *qElem = NULL;
    qElem = new Queue::QueueNode(element);
    //NOT ALLOCATED
    if (qElem == NULL) {
      return CAPU_ENO_MEMORY;
    }

    //Queue contains some elements so add it to the end of queue
    if (mCurrent != NULL) {
      mTail->mNext = qElem;
      mTail = qElem;
      ++mSize;
      return CAPU_OK;
    }
    //Queue is empty
    mCurrent = qElem;
    mTail = qElem;
    ++mSize;
    return CAPU_OK;
  }

 //checks if the queue is empty or not
  template <typename T>
  bool_t Queue<T>::isEmpty() {
    return (mSize == 0);
  }
}


#endif /* __QUEUE_H__ */

