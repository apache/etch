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
#include "capu/os/Thread.h"


class ThreadTest : public capu::Runnable
{
  public:
    static capu::int32_t variable;

    ThreadTest(capu::int32_t val)
    {
        variable = 0;
        mVal = val;
    }

    void run()
    {
        //1 second delay
        EXPECT_TRUE(capu::Thread::Sleep(1000)==capu::CAPU_OK);
        variable = mVal;
    }
  private:
    capu::int32_t mVal;
};

class ThreadTest2 : public capu::Runnable
{
  public:

    ThreadTest2()
    {
    }

    void run()
    {
      capu::Thread::Sleep(1000);
      return;
    }
};


capu::int32_t ThreadTest::variable;

TEST(Thread,startAndJoinTest)
{
    ThreadTest _test(6);
    ThreadTest2 _test2;
    //CREATE THREAD
    capu::Thread * CAPU_thread = new capu::Thread(&_test);
    CAPU_thread->start();
    //WAIT UNTIL IT FINISH
    EXPECT_TRUE(CAPU_thread->join()==capu::CAPU_OK);
    //CHECK THE VALUE CALCULATED IN THREAD
    EXPECT_TRUE(ThreadTest::variable == 6);
    delete CAPU_thread;
    capu::Thread * CAPU_thread2 = new capu::Thread(&_test2);
    CAPU_thread2->start();
    EXPECT_TRUE(CAPU_thread2->join()==capu::CAPU_OK);
    delete CAPU_thread2;
}

TEST(Thread,startAndDestructorTest)
{
    capu::int32_t newval = 6;
    ThreadTest _test(newval);

    capu::Thread * CAPU_thread = new capu::Thread(&_test);
    CAPU_thread->start();
    delete CAPU_thread;

    EXPECT_TRUE(ThreadTest::variable == newval);
}


TEST(Thread,sleepTest)
{
    capu::int32_t newval = 5;
    ThreadTest _test(newval);
    //CREATE THREAD
    capu::Thread * CAPU_thread = new capu::Thread(&_test);
    CAPU_thread->start();
    //WAIT UNTIL IT FINISH
    EXPECT_TRUE(CAPU_thread->join()==capu::CAPU_OK);
    EXPECT_TRUE(ThreadTest::variable == newval);
    delete CAPU_thread;
}

TEST(Thread,contextTest)
{
  capu::int32_t newval = 4;
  ThreadTest _test(newval);
  {
    capu::Thread thread(&_test);
    thread.start();
  }
  EXPECT_TRUE(ThreadTest::variable == newval);
}

TEST(Thread, getState)
{
    ThreadTest2 r1;
    capu::Thread thread(&r1);
    capu::ThreadState state = thread.getState();
    EXPECT_EQ(capu::TS_NEW, state);
    thread.start();
    capu::Thread::Sleep(500);
    state = thread.getState();
    EXPECT_EQ(capu::TS_RUNNING, state);
    thread.join();
    state = thread.getState();
    EXPECT_EQ(capu::TS_TERMINATED, state);
}
