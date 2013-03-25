/* $Id$
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

#include <gtest/gtest.h>
#include "capu/os/Thread.h"
#include "util/EtchCircularQueue.h"

TEST(EtchCirclerQueueTest, constructorTest) {
  EtchCircularQueue *queue = NULL;
  queue = new EtchCircularQueue(25);
  EXPECT_TRUE(queue != NULL);
  EXPECT_TRUE(queue->getSize() == 25);
  delete queue;
  queue = new EtchCircularQueue();
  EXPECT_TRUE(queue->getSize() == 10);
  delete queue;
}

TEST(EtchCirclerQueueTest, putAndGetTest) {
  EtchCircularQueue* queue = new EtchCircularQueue;

  EtchMailbox::EtchElement* e1 = NULL;

  EXPECT_EQ(ETCH_EINVAL, queue->get(&e1, -1));
  EXPECT_EQ(ETCH_TIMEOUT, queue->get(&e1, 1));

  EXPECT_EQ(0u, queue->getCount());
  EXPECT_TRUE(queue->isEmpty());
  EXPECT_FALSE(queue->isFull());

  for (capu::int32_t i = 0; i < 100000; i++) {
    EtchMailbox::EtchElement* e2 = new EtchMailbox::EtchElement(NULL, NULL);
    EtchMailbox::EtchElement* e3 = NULL;

    EXPECT_EQ(ETCH_OK, queue->put(e2));
    EXPECT_FALSE(queue->isEmpty());

    EXPECT_EQ(ETCH_OK, queue->get(&e3));
    EXPECT_EQ(e2, e3);
    EXPECT_EQ(0u, queue->getCount());
    EXPECT_TRUE(queue->isEmpty());
    delete e2;
  }

  EtchMailbox::EtchElement * e4 = new EtchMailbox::EtchElement(NULL, NULL);
  EtchMailbox::EtchElement * e5 = new EtchMailbox::EtchElement(NULL, NULL);
  EtchMailbox::EtchElement * e6 = new EtchMailbox::EtchElement(NULL, NULL);
  EtchMailbox::EtchElement * e7 = NULL;

  // default get
  EXPECT_EQ(ETCH_OK, queue->put(e4));
  EXPECT_EQ(ETCH_OK, queue->put(e5));
  EXPECT_EQ(ETCH_OK, queue->put(e6));
  EXPECT_EQ(ETCH_OK, queue->get(&e7));
  EXPECT_EQ(e4, e7);
  e7 = NULL;
  EXPECT_EQ(ETCH_OK, queue->get(&e7));
  EXPECT_EQ(e5, e7);
  e7 = NULL;
  EXPECT_EQ(ETCH_OK, queue->get(&e7));
  EXPECT_EQ(e6, e7);
  e7 = NULL;

  // timeout get
  EXPECT_EQ(ETCH_OK, queue->put(e4, -1));
  EXPECT_EQ(ETCH_OK, queue->put(e5, -1));
  EXPECT_EQ(ETCH_OK, queue->put(e6, -1));
  EXPECT_EQ(ETCH_OK, queue->get(&e7, -1));
  EXPECT_EQ(e4, e7);
  e7 = NULL;
  EXPECT_EQ(ETCH_OK, queue->get(&e7, -1));
  EXPECT_EQ(e5, e7);
  e7 = NULL;
  EXPECT_EQ(ETCH_OK, queue->get(&e7, -1));
  EXPECT_EQ(e6, e7);
  e7 = NULL;

  // timeout get
  EXPECT_EQ(ETCH_OK, queue->put(e4, 1));
  EXPECT_EQ(ETCH_OK, queue->put(e5, 1));
  EXPECT_EQ(ETCH_OK, queue->put(e6, 1));
  EXPECT_EQ(ETCH_OK, queue->get(&e7, 1));
  EXPECT_EQ(e4, e7);
  e7 = NULL;
  EXPECT_EQ(ETCH_OK, queue->get(&e7, 1));
  EXPECT_EQ(e5, e7);
  e7 = NULL;
  EXPECT_EQ(ETCH_OK, queue->get(&e7, 1));
  EXPECT_EQ(e6, e7);
  e7 = NULL;

  delete e4;
  delete e5;
  delete e6;
  delete queue;
}

TEST(EtchCirclerQueueTest, fullTest) {
  EtchCircularQueue* queue = new EtchCircularQueue(1);

  EtchMailbox::EtchElement* e1 = new EtchMailbox::EtchElement(NULL, NULL);
  EtchMailbox::EtchElement* e2 = NULL;

  EXPECT_FALSE(queue->isFull());
  EXPECT_EQ(ETCH_OK, queue->put(e1));
  EXPECT_TRUE(queue->isFull());
  EXPECT_EQ(ETCH_OK, queue->get(&e2));
  EXPECT_EQ(e1, e2);
  e2 = NULL;
  EXPECT_FALSE(queue->isFull());

  // queue overrun
  EXPECT_EQ(ETCH_OK, queue->put(e1));
  EXPECT_EQ(ETCH_EINVAL, queue->put(e1, -1));
  EXPECT_EQ(ETCH_TIMEOUT, queue->put(e1, 1));
  EXPECT_EQ(ETCH_OK, queue->get(&e2));
  EXPECT_EQ(e1, e2);
  e2 = NULL;

  delete e1;
  delete queue;
}

TEST(EtchCirclerQueueTest, closeTest) {
  EtchCircularQueue* queue = new EtchCircularQueue(1);

  EtchMailbox::EtchElement * e1 = new EtchMailbox::EtchElement(NULL, NULL);
  EtchMailbox::EtchElement * e2 = NULL;

  EXPECT_FALSE(queue->isClosed());
  queue->close();
  EXPECT_TRUE(queue->isClosed());

  EXPECT_EQ(ETCH_ERROR, queue->get(&e2));
  EXPECT_EQ(ETCH_EINVAL, queue->get(&e2, -1));
  EXPECT_EQ(ETCH_ERROR, queue->get(&e2, 0));
  EXPECT_EQ(ETCH_ERROR, queue->get(&e2, 1));

  delete e1;
  delete queue;
}

namespace {

class Runnable1 : public capu::Runnable {
public:
  Runnable1(EtchCircularQueue* queue) {
    mQueue = queue;
  }

  void run() {
    // Producer
    for(capu::int32_t i = 0; i < 2000; i++) {
      EtchMailbox::EtchElement * e1 = new EtchMailbox::EtchElement(NULL, NULL);
      EXPECT_EQ(ETCH_OK, mQueue->put(e1));
    }
  }

private:
  EtchCircularQueue* mQueue;
};

class Runnable2 : public capu::Runnable {
public:
  Runnable2(EtchCircularQueue* queue) {
    mQueue = queue;
  }

  void run() {
    // Consumer
    for(capu::int32_t i = 0; i < 2000; i++) {
      EtchMailbox::EtchElement * e1 = NULL;
      EXPECT_EQ(ETCH_OK, mQueue->get(&e1));
      delete e1;
    }
  }

private:
  EtchCircularQueue* mQueue;
};

}

TEST(EtchCirclerQueueTest, concurrency) {
  EtchCircularQueue* queue = new EtchCircularQueue(5);

  Runnable1* r1 = new Runnable1(queue);
  capu::Thread* t1 = new capu::Thread();
  t1->start(*r1);

  Runnable2* r2 = new Runnable2(queue);
  capu::Thread* t2 = new capu::Thread();
  t2->start(*r2);

  t1->join();
  t2->join();

  delete r1;
  delete r2;
  delete t1;
  delete t2;
  delete queue;
}
