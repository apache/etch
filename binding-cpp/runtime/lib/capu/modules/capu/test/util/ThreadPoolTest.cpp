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
0 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "capu/util/ThreadPool.h"
#include "capu/os/Mutex.h"
#include "capu/os/AtomicOperation.h"

class Globals {
public:
  static capu::uint32_t var;
};
capu::uint32_t Globals::var = 0;

class WorkToDo : public capu::Runnable {
public:
  WorkToDo() {

  }

  void run() {
    capu::AtomicOperation::AtomicAdd32(Globals::var, 5);
  }
};

TEST(ThreadPool, ConstructorTest) {
  capu::ThreadPool* pool = new capu::ThreadPool();
  EXPECT_TRUE(pool != NULL);
  EXPECT_EQ(5, pool->getSize());
  delete pool;
  pool = new capu::ThreadPool(10);
  EXPECT_TRUE(pool != NULL);
  EXPECT_EQ(10, pool->getSize());
  delete pool;
}

TEST(ThreadPool, AddJoinTest) {
  capu::ThreadPool* pool = new capu::ThreadPool();
  for (capu::int32_t i = 0; i<100000; i++) {
    capu::SmartPointer<WorkToDo> w = new WorkToDo();
    EXPECT_EQ(capu::CAPU_OK, pool->add(w));
  }
  EXPECT_EQ(capu::CAPU_OK, pool->join());
  //check if all the work has been done which means that all threads have been executed
  EXPECT_EQ(500000u, Globals::var);

  //make sure adding is no more supported after pool has been closed
  capu::SmartPointer<WorkToDo> w = new WorkToDo();
  EXPECT_EQ(capu::CAPU_ERROR, pool->add(w));

  delete pool;
}
