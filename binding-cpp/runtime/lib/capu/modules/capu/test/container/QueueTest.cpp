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
#include <gtest/gtest.h>
#include "capu/container/Queue.h"

TEST(Queue, Constructor_Default) {
  //create an empty linked Queue
  capu::Queue<capu::int32_t*>* queue = NULL;
  queue = new capu::Queue<capu::int32_t*>();
  EXPECT_TRUE(queue != NULL);
  delete queue;
}

TEST(Queue, add_remove_Test) {
  capu::Queue<capu::int32_t>* Queue = new capu::Queue<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::int32_t test;
  capu::status_t result;

  data1 = 32;
  data2 = 43;
  data3 = 1211;
  //append element to the linked Queue
  result = Queue->add(data1);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //append another element to linked Queue
  result = Queue->add(data2);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = Queue->add(data3);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //Empty Queue, and see if it works correctly
  //First Element
  capu::bool_t hasNext = Queue->hasNext();
  EXPECT_TRUE(hasNext);
  EXPECT_EQ(capu::CAPU_OK, Queue->next(&test));
  EXPECT_TRUE(test == data1);
  //Second Element
  hasNext = Queue->hasNext();
  EXPECT_TRUE(hasNext);
  EXPECT_EQ(capu::CAPU_OK, Queue->next(&test));
  EXPECT_TRUE(test == data2);
  //Third Element
  hasNext = Queue->hasNext();
  EXPECT_TRUE(hasNext);
  EXPECT_EQ(capu::CAPU_OK, Queue->next(&test));
  EXPECT_TRUE(test == data3);
  //Should be Empty
  hasNext = Queue->hasNext();
  EXPECT_FALSE(hasNext);

  delete Queue;
}

TEST(Queue, size) {
  capu::Queue<capu::int32_t>* Queue = new capu::Queue<capu::int32_t > ();
  capu::int32_t data1;

  capu::int_t result;

  data1 = 32;

  //size of empty Queue
  result = Queue->size();
  EXPECT_TRUE(result == 0);

  //add some elements and check the size for each step
  Queue->add(data1);
  result = Queue->size();
  EXPECT_TRUE(result == 1);

  Queue->add(data1);
  result = Queue->size();
  EXPECT_TRUE(result == 2);

  Queue->add(data1);
  result = Queue->size();
  EXPECT_TRUE(result == 3);

  Queue->next(NULL);
  result = Queue->size();
  EXPECT_TRUE(result == 2);

  Queue->next();
  result = Queue->size();
  EXPECT_TRUE(result == 1);

  delete Queue;
}

TEST(Queue, empty) {
  capu::Queue<capu::int32_t>* Queue = new capu::Queue<capu::int32_t > ();
  capu::int32_t data1;

  capu::bool_t result;

  data1 = 32;

  //check the empty Queue
  result = Queue->isEmpty();
  EXPECT_TRUE(result);

  //add some element
  Queue->add(data1);
  result = Queue->isEmpty();
  EXPECT_FALSE(result);

  delete Queue;
}


TEST(Queue, clear) {
  capu::Queue<capu::int32_t>* Queue = new capu::Queue<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::status_t result;

  data1 = 32;
  data2 = 43;
  data3 = 44;

  //add some dummy values to the lQueue
  Queue->add(data1);
  Queue->add(data1);
  Queue->add(data2);
  Queue->add(data3);

  //remove all elements from the linked Queue
  result = Queue->clear();
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = Queue->add(data1);
  EXPECT_TRUE(result == capu::CAPU_OK);


  capu::int32_t test;
  EXPECT_EQ(capu::CAPU_OK, Queue->next(&test));
  EXPECT_TRUE(data1 == test);
  delete Queue;
}


TEST(Queue, next) {
  capu::Queue<capu::int32_t>* Queue = new capu::Queue<capu::int32_t > ();
  capu::int32_t test;
  capu::int32_t data1;
  capu::int32_t data2;

  int cnt = 0;
  EXPECT_FALSE(Queue->hasNext());
  data1 = 32;
  data2 = 43;
  Queue->add(data1);
  Queue->add(data2);

  while (Queue->hasNext()) {
    EXPECT_EQ(capu::CAPU_OK, Queue->next(&test));
    if (cnt == 0) {
      EXPECT_TRUE(test == data1);
    } else {
      EXPECT_TRUE(test == data2);
    }
    cnt++;
  }
  delete Queue;
}

