/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


#include <gtest/gtest.h>
#include "capu/os/Thread.h"
#include "support/EtchMonitor.h"

TEST(EtchMonitorTest, constructorTest) {
  EtchMonitor *m = new EtchMonitor(EtchString("desc"), EtchString("init"));
  EXPECT_TRUE(m != NULL);
  delete m;
}

TEST(EtchMonitorTest, setTest) {
  EtchString init("init");
  EtchMonitor *m = new EtchMonitor(EtchString("desc"), init);
  EtchString tmp1("tmp1");
  EtchString tmp2("tmp2");
  EtchString tmp3("tmp3");
  EtchString old;
  EXPECT_TRUE(m != NULL);

  m->set(tmp1, old);
  EXPECT_TRUE(old.equals(&init));
  m->set(tmp2, old);
  EXPECT_TRUE(old.equals(&tmp1));
  m->set(tmp3, old);
  EXPECT_TRUE(old.equals(&tmp2));

  delete m;
}

TEST(EtchMonitorTest, waitUntilEqTest) {
  EtchString init("init");
  EtchMonitor *m = new EtchMonitor(EtchString("desc"), init);
  EtchString tmp1("tmp1");
  EtchString tmp2("tmp2");
  EtchString old;

  EXPECT_EQ(ETCH_OK, m->waitUntilEq(init));
  EXPECT_EQ(ETCH_OK, m->waitUntilEq(init, 0));
  m->set(tmp1, old);
  EXPECT_EQ(ETCH_OK, m->waitUntilEq(tmp1));
  EXPECT_EQ(ETCH_OK, m->waitUntilEq(tmp1, 0));

  m->set(tmp2, old);
  EXPECT_EQ(ETCH_OK, m->waitUntilEq(tmp2));
  EXPECT_EQ(ETCH_OK, m->waitUntilEq(tmp2, 0));
  EXPECT_EQ(ETCH_TIMEOUT, m->waitUntilEq(tmp1, 2000));
  delete m;
}

namespace {

class Runnable1 : public capu::Runnable {
public:
  Runnable1(EtchMonitor *monitor) {
    mMonitor = monitor;
  }

  void run() {
    capu::Thread::Sleep(2000);
    EtchString str;
    mMonitor->set("tmp1", str);
  }

private:
  EtchMonitor *mMonitor;
};

}

TEST(EtchMonitorTest, waitUntilNotEqTest) {
  EtchString init("init");
  EtchMonitor *m = new EtchMonitor(EtchString("desc"), init);
  EtchString tmp1("tmp1");
  EtchString tmp2("tmp2");
  EtchString current;

  EXPECT_EQ(ETCH_OK, m->waitUntilNotEq(tmp1, current));
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEq(tmp1, 0, current));
  m->set(tmp1, current);
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEq(tmp2, current));
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEq(tmp2, 0, current));

  // blocking test
  m->set(tmp2, current);
  Runnable1* r1 = new Runnable1(m);
  capu::Thread* t1 = new capu::Thread();
  t1->start(*r1);
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEq(tmp2, current));
  t1->join();
  delete r1;
  delete t1;

  // blocking timeout test
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEq(tmp2, 2000, current));
  EXPECT_EQ(ETCH_TIMEOUT, m->waitUntilNotEq(tmp1, 2000, current));

  delete m;
}

TEST(EtchMonitorTest, waitUntilEqAndSetTest) {
  EtchString init("init");
  EtchMonitor *m = new EtchMonitor(EtchString("desc"), init);
  EtchString tmp1("tmp1");
  EtchString tmp2("tmp2");
  EtchString current;

  EXPECT_EQ(ETCH_OK, m->waitUntilEqAndSet(init, tmp1, current));
  EXPECT_EQ(ETCH_OK, m->waitUntilEqAndSet(tmp1, 0, tmp1, current));
  m->set(tmp1, current);
  EXPECT_EQ(ETCH_OK, m->waitUntilEqAndSet(tmp1, tmp2, current));
  EXPECT_EQ(ETCH_OK, m->waitUntilEqAndSet(tmp2, 0, tmp2, current));

  m->set(tmp2, current);
  // blocking test
  m->set(tmp2, current);
  Runnable1* r1 = new Runnable1(m);
  capu::Thread* t1 = new capu::Thread();
  t1->start(*r1);
  EXPECT_EQ(ETCH_OK, m->waitUntilEqAndSet(tmp1, tmp2, current));
  t1->join();
  delete r1;
  delete t1;

  EXPECT_EQ(ETCH_TIMEOUT, m->waitUntilEqAndSet(init, 2000, tmp2, current));
  EXPECT_EQ(ETCH_OK, m->waitUntilEqAndSet(tmp2, 2000, tmp2, current));

  delete m;
}

TEST(EtchMonitorTest, waitUntilNotEqAndSetTest) {
  EtchString init("init");
  EtchMonitor *m = new EtchMonitor(EtchString("desc"), init);
  EtchString tmp1("tmp1");
  EtchString tmp2("tmp2");
  EtchString current;

  EXPECT_EQ(ETCH_OK, m->waitUntilNotEqAndSet(tmp1, init, current));
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEqAndSet(tmp1, 0, init, current));
  m->set(tmp1, current);
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEqAndSet(tmp2, tmp1, current));
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEqAndSet(tmp2, 0, tmp2, current));

  m->set(tmp2, current);
  // blocking test
  m->set(tmp2, current);
  Runnable1* r1 = new Runnable1(m);
  capu::Thread* t1 = new capu::Thread();
  t1->start(*r1);
  EXPECT_EQ(ETCH_OK, m->waitUntilNotEqAndSet(tmp2, tmp1, current));
  t1->join();
  delete r1;
  delete t1;

  EXPECT_EQ(ETCH_OK, m->waitUntilNotEqAndSet(tmp2, 2000, tmp2, current));
  EXPECT_EQ(ETCH_TIMEOUT, m->waitUntilNotEqAndSet(tmp2, 2000, tmp1, current));

  delete m;
}
