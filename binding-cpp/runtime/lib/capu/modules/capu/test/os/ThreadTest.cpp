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

    ThreadTest()
    {
        variable=0;
    }

    inline void operator()(void * param)
    {
        capu::int32_t * val =(capu::int32_t*) param;
        //1 second delay
        EXPECT_TRUE(capu::Thread::Sleep(1000)==capu::CAPU_OK);
        variable = *val;
    }
};

class ThreadTest2 : public capu::Runnable
{
  public:

    ThreadTest2()
    {
    }

    inline void operator()(void * param)
    {
      return;
    }
};


capu::int32_t ThreadTest::variable;

TEST(Thread,startAndJoinTest)
{
    ThreadTest _test;
    ThreadTest2 _test2;
    capu::int32_t newval = 6;
    //CREATE THREAD
    capu::Thread * CAPU_thread = new capu::Thread(&_test,(void *)&newval);
    //WAIT UNTIL IT FINISH
    EXPECT_TRUE(CAPU_thread->join()==capu::CAPU_OK);
    //CHECK THE VALUE CALCULATED IN THREAD
    EXPECT_TRUE(ThreadTest::variable == newval);
    delete CAPU_thread;
    capu::Thread * CAPU_thread2 = new capu::Thread(&_test2,NULL);
    EXPECT_TRUE(CAPU_thread2->join()==capu::CAPU_OK);
    delete CAPU_thread2;
}

TEST(Thread,startAndDestructorTest)
{
    ThreadTest _test;
    capu::int32_t newval = 6;

    capu::Thread * CAPU_thread = new capu::Thread(&_test,(void *)&newval);
    delete CAPU_thread;

    EXPECT_TRUE(ThreadTest::variable == newval);
}


TEST(Thread,sleepTest)
{
    capu::int32_t newval=5;
    ThreadTest _test;
    //CREATE THREAD
    capu::Thread * CAPU_thread = new capu::Thread(&_test,(void *)&newval);
    //WAIT UNTIL IT FINISH
    EXPECT_TRUE(CAPU_thread->join()==capu::CAPU_OK);
    EXPECT_TRUE(ThreadTest::variable == newval);
    delete CAPU_thread;
}

TEST(Thread,contextTest)
{
    ThreadTest _test;
    capu::int32_t newval =4;
    {
        capu::Thread thread(&_test,(void*)&newval);
    }
    EXPECT_TRUE(ThreadTest::variable == newval);
}
