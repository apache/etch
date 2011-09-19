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
    static capu::int32_t variable;
    static capu::CondVar cv;
    static capu::Mutex mutex;
};

capu::int32_t GlobalVariables::variable=10;
capu::CondVar GlobalVariables::cv;
capu::Mutex GlobalVariables::mutex;

class ThreadCondVarConsumer
{
public:

    inline void operator()(void * param)
    {
        capu::int32_t *value = (capu::int32_t *) param;
        EXPECT_TRUE(GlobalVariables::mutex.lock()== capu::CAPU_OK);
        //wait for new value
        EXPECT_TRUE(GlobalVariables::cv.wait(&GlobalVariables::mutex)== capu::CAPU_OK);
        //add value to new value
        GlobalVariables::variable = GlobalVariables::variable + *value;
        EXPECT_TRUE(GlobalVariables::mutex.unlock()== capu::CAPU_OK);
    }
};

class ThreadCondVarProducerBroadcast
{
 public:
    inline void operator()(void * param)
    {
        //delayto make sure that producer will be called after consumer started
        capu::Thread::Sleep(1000);
        EXPECT_TRUE(GlobalVariables::mutex.lock()== capu::CAPU_OK);
        //update shared value
        GlobalVariables::variable = 5;
        EXPECT_TRUE(GlobalVariables::cv.broadcast()== capu::CAPU_OK);
        EXPECT_TRUE(GlobalVariables::mutex.unlock()== capu::CAPU_OK);
    }
};

class ThreadCondVarProducerSignal
{
 public:
    inline void operator()(void * param)
    {
        //delayto make sure that producer will be called after consumer started
        capu::Thread::Sleep(1000);
        EXPECT_TRUE(GlobalVariables::mutex.lock()== capu::CAPU_OK);
        //update shared value
        GlobalVariables::variable = 5;
        EXPECT_TRUE(GlobalVariables::cv.signal()== capu::CAPU_OK);
        EXPECT_TRUE(GlobalVariables::mutex.unlock()== capu::CAPU_OK);
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
    capu::int32_t newval = 6;
    ThreadCondVarConsumer consumer;
    ThreadCondVarProducerBroadcast producer;
    //create two thread
    capu::Thread * CAPU_thread = new capu::Thread(&consumer,&newval);
    capu::Thread * CAPU_thread2 = new capu::Thread(&consumer,&newval);
    capu::Thread * CAPU_thread3 = new capu::Thread(&consumer,&newval);
    capu::Thread * CAPU_thread4 = new capu::Thread(&producer,&newval);

    //wait for them to finish
    CAPU_thread->join();
    CAPU_thread2->join();
    CAPU_thread3->join();
    CAPU_thread4->join();

    //expected value
    EXPECT_EQ(23, GlobalVariables::variable);
    delete CAPU_thread;
    delete CAPU_thread2;
    delete CAPU_thread3;
    delete CAPU_thread4;
}

TEST(CondVar,WaitAndSignalTest)
{
    capu::int32_t newval = 6;
    ThreadCondVarConsumer consumer;
    ThreadCondVarProducerSignal producer;
    //create two thread
    capu::Thread * CAPU_thread = new capu::Thread(&consumer,&newval);
    capu::Thread * CAPU_thread2 = new capu::Thread(&producer,&newval);
    //wait for them to finish
    CAPU_thread->join();
    CAPU_thread2->join();
    //expected value
    EXPECT_EQ(11, GlobalVariables::variable);
    delete CAPU_thread;
    delete CAPU_thread2;
}
