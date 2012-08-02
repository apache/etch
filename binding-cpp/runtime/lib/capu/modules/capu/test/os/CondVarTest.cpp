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
#include "capu/os/CondVar.h"
#include "capu/os/Thread.h"

class GlobalVariables
{
public:
    static capu::int32_t loops;
    static capu::int32_t buffer;
    static capu::CondVar cv;
    static capu::Mutex mutex;
};

capu::int32_t GlobalVariables::loops = 10000;
capu::CondVar GlobalVariables::cv;
capu::Mutex GlobalVariables::mutex;
capu::int32_t GlobalVariables::buffer = 0;

class ThreadCondVarConsumer : public capu::Runnable
{
public:
    void run()
    {
      for(capu::int32_t i = 0; i < GlobalVariables::loops; i++) {
        EXPECT_TRUE(GlobalVariables::mutex.lock()== capu::CAPU_OK);
        while(GlobalVariables::buffer == 0) {
          EXPECT_TRUE(GlobalVariables::cv.wait(&GlobalVariables::mutex)== capu::CAPU_OK);
        }
        GlobalVariables::buffer--;
        EXPECT_TRUE(GlobalVariables::cv.signal()== capu::CAPU_OK);
        EXPECT_TRUE(GlobalVariables::mutex.unlock()== capu::CAPU_OK);
      }
    }
};

class ThreadCondVarProducerBroadcast : public capu::Runnable
{
 public:
    void run()
    {
      for(capu::int32_t i = 0; i < GlobalVariables::loops * 3; i++) {
        EXPECT_TRUE(GlobalVariables::mutex.lock()== capu::CAPU_OK);
        while(GlobalVariables::buffer == 100) {
          EXPECT_TRUE(GlobalVariables::cv.wait(&GlobalVariables::mutex)== capu::CAPU_OK);
        }
        GlobalVariables::buffer++;
        EXPECT_TRUE(GlobalVariables::cv.broadcast()== capu::CAPU_OK);
        EXPECT_TRUE(GlobalVariables::mutex.unlock()== capu::CAPU_OK);
      }
    }
};

class ThreadCondVarProducerSignal : public capu::Runnable
{
 public:
    void run()
    {
      for(capu::int32_t i = 0; i < GlobalVariables::loops; i++) {
        EXPECT_TRUE(GlobalVariables::mutex.lock()== capu::CAPU_OK);
        while(GlobalVariables::buffer == 100) {
          EXPECT_EQ(capu::CAPU_OK, GlobalVariables::cv.wait(&GlobalVariables::mutex));
        }
        GlobalVariables::buffer++;
        EXPECT_TRUE(GlobalVariables::cv.signal()== capu::CAPU_OK);
        EXPECT_TRUE(GlobalVariables::mutex.unlock()== capu::CAPU_OK);
      }
    }
};

TEST(CondVar,ConstructorTest)
{
    capu::CondVar *_cv = new capu::CondVar();
    EXPECT_TRUE(_cv != NULL);
    delete _cv;
}

TEST(CondVar,WaitTest)
{
    capu::CondVar *_cv = new capu::CondVar();
    EXPECT_TRUE(_cv->wait(NULL) == capu::CAPU_EINVAL);
    delete _cv;
}

TEST(CondVar,WaitAndBroadcastTest)
{
    ThreadCondVarConsumer consumer;
    ThreadCondVarProducerBroadcast producer;
    //create two thread
    capu::Thread * CAPU_thread = new capu::Thread(&consumer);
    CAPU_thread->start();
    capu::Thread * CAPU_thread2 = new capu::Thread(&consumer);
    CAPU_thread2->start();
    capu::Thread * CAPU_thread3 = new capu::Thread(&consumer);
    CAPU_thread3->start();
    capu::Thread * CAPU_thread4 = new capu::Thread(&producer);
    CAPU_thread4->start();

    //wait for them to finish
    CAPU_thread->join();
    CAPU_thread2->join();
    CAPU_thread3->join();
    CAPU_thread4->join();

    //expected value
    EXPECT_EQ(0, GlobalVariables::buffer);
    delete CAPU_thread;
    delete CAPU_thread2;
    delete CAPU_thread3;
    delete CAPU_thread4;
}

TEST(CondVar,WaitAndSignalTest)
{
    ThreadCondVarConsumer consumer;
    ThreadCondVarProducerSignal producer;
    //create two thread
    capu::Thread * CAPU_thread = new capu::Thread(&consumer);
    CAPU_thread->start();
    capu::Thread * CAPU_thread2 = new capu::Thread(&producer);
    CAPU_thread2->start();
    //wait for them to finish
    CAPU_thread->join();
    CAPU_thread2->join();
    //expected value
    EXPECT_EQ(0, GlobalVariables::buffer);
    delete CAPU_thread;
    delete CAPU_thread2;
}


TEST(CondVar,TimedWaitTest)
{
    capu::Mutex mutex;
    capu::CondVar condvar;
    mutex.lock();
    //2 second wait and timeout
    EXPECT_TRUE(condvar.wait(&mutex, 2000) == capu::CAPU_ETIMEOUT);
    mutex.unlock();
}

//TODO add recursive mutext condvar test
