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
#include "capu/os/CondVar.h"
#include "capu/os/Thread.h"
#include "support/EtchFreePool.h"

class EtchFreePooTestRunnable : public EtchPoolRunnable{
public:


  EtchFreePooTestRunnable(capu::Mutex* mutex, capu::CondVar* condVar, capu::int8_t* cond)
    : mMutex(mutex), mCondVar(condVar), mCond(cond) {
  }

  /**
   * @see EtchPoolRunnable
   */
  status_t run() {
    //printf("run\n");
    mMutex->lock();

    // case 1
    while(*mCond == 1 || *mCond == 2) {
      (*mCond)++;
      //printf("wait\n");
      mCondVar->wait(mMutex);
      //printf("wait -> run\n");
    }

    mMutex->unlock();
    return ETCH_OK;
  }

  /**
   * @see EtchPoolRunnable
   */
  status_t exception(capu::SmartPointer<EtchException> exception) {
    return ETCH_OK;
  }

private:
  capu::Mutex* mMutex;
  capu::CondVar* mCondVar;
  capu::int8_t* mCond;
};


TEST(EtchFreePoolTest, Constructor_Default){
  EtchFreePool* pool = new EtchFreePool(20);
  delete pool;
}

TEST(EtchFreePoolTest, add){
  EtchFreePool* pool = new EtchFreePool(2);

  capu::Mutex mutex;
  capu::CondVar condVar;
  capu::int8_t cond;

  capu::SmartPointer<EtchFreePooTestRunnable>* ra = new capu::SmartPointer<EtchFreePooTestRunnable>[10];
  for(int i = 0; i < 10; i++) {
    ra[i] = new EtchFreePooTestRunnable(&mutex, &condVar, &cond);
  }

  mutex.lock();
  cond = 1;
  mutex.unlock();

  status_t status;
  status = pool->add(ra[0]);
  EXPECT_EQ(ETCH_OK, status);
  status = pool->add(ra[1]);
  EXPECT_EQ(ETCH_OK, status);

  // wait if pool is full
  mutex.lock();
  while(cond < 3) {
    condVar.wait(&mutex, 1000);
  }
  mutex.unlock();

  // pool is full, so error
  status = pool->add(ra[2]);
  EXPECT_EQ(ETCH_ERROR, status);

  // set new cond and signal
  mutex.lock();
  cond = 4;
  condVar.broadcast();
  mutex.unlock();

  // wait if pool is empty
  mutex.lock();
  while(pool->getSize() > 0) {
    condVar.wait(&mutex, 1000);
  }
  mutex.unlock();

  status = pool->add(ra[3]);
  EXPECT_EQ(ETCH_OK, status);

  pool->join();
  delete pool;
  delete[] ra;
}

TEST(EtchFreePoolTest, join){
  EtchFreePool* pool = new EtchFreePool(20);
  pool->join();
  delete pool;
}

