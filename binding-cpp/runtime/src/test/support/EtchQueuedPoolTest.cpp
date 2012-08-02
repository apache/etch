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

#include "capu/os/Mutex.h"
#include "support/EtchQueuedPool.h"

class EtchQueuedPooTestRunnable : public EtchPoolRunnable{
public:


  EtchQueuedPooTestRunnable()
    : mCount(0) {
  }

  /**
   * @see EtchPoolRunnable
   */
  status_t run() {
    mMutex.lock();
    mCount++;
    mMutex.unlock();
    return ETCH_OK;
  }

  /**
   * @see EtchPoolRunnable
   */
  status_t exception(capu::SmartPointer<EtchException> exception) {
    return ETCH_OK;
  }

  /**
   * Returns count
   */
  capu::int32_t getCount() {
    return mCount;
  }

private:
  capu::Mutex mMutex;
  capu::int32_t mCount;
};

TEST(EtchQueuedPoolTest, Constructor_Default){
  EtchQueuedPool* pool = new EtchQueuedPool(2);
  delete pool;
}

TEST(EtchQueuedPoolTest, add){
  EtchQueuedPool* pool = new EtchQueuedPool();

  capu::SmartPointer<EtchQueuedPooTestRunnable> ra = new EtchQueuedPooTestRunnable();

  for(int i = 0; i < 10000; i++) {
    status_t status = pool->add(ra);
    ASSERT_EQ(ETCH_OK, status);
  }
  pool->join();
  ASSERT_EQ(10000, ra->getCount());
  delete pool;
}

TEST(EtchQueuedPoolTest, close){
  EtchQueuedPool* pool = new EtchQueuedPool();
  delete pool;
}

TEST(EtchQueuedPoolTest, join){
  EtchQueuedPool* pool = new EtchQueuedPool();
  delete pool;
}

