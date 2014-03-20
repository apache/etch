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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "support/EtchRuntime.h"
#include "capu/util/LogMessage.h"

class MockLogAppender : public IEtchLogAppender {
public:
  virtual ~MockLogAppender() {};

  MOCK_METHOD1(log, void(const capu::LogMessage& message));

};

capu::uint64_t currentIdCount;

TEST(EtchRuntime, Default) {
  EtchRuntime* runtime = new EtchRuntime(); //ID = 0
  currentIdCount = runtime->getId();
  delete runtime;
}

TEST(EtchRuntime, Logging) {
  MockLogAppender* logAppender = new MockLogAppender();

  EtchRuntime* runtime = new EtchRuntime(*logAppender, capu::LL_ALL); //ID = 2

  EXPECT_CALL(*logAppender, log(::testing::_)).Times(2);

  EtchLogContext testContext = runtime->getLogger().createContext("testcontext");
  testContext.setLogLevel(capu::LL_ALL);

  ETCH_LOG_DEBUG(runtime->getLogger(), testContext, "test log message for runtime " << runtime->getId());
  ETCH_LOG_DEBUG(runtime->getLogger(), runtime->getLogger().getMessagizerContext(), "test log message for runtime " << runtime->getId());

  delete runtime;
  delete logAppender;
}

TEST(EtchRuntime, getId) {
  EtchRuntime* runtime = new EtchRuntime(); //ID = 3
  EXPECT_EQ(currentIdCount + 2, runtime->getId());
  delete runtime;
  runtime = new EtchRuntime(); //ID = 4
  EXPECT_EQ(currentIdCount + 3, runtime->getId());
  delete runtime;
}

TEST(EtchRuntime, isClosed) {
  EtchRuntime* runtime = new EtchRuntime();
  EXPECT_FALSE(runtime->isClosed());
  runtime->shutdown();
  EXPECT_TRUE(runtime->isClosed());
  delete runtime;
}

class EtchRuntimeTestListener : public EtchRuntimeListener {
public:
  /**
   * Constructor
   */
  EtchRuntimeTestListener()
    : mId(0) {
  }

  /**
   * @see EtchRuntimeListener
   */
  status_t onRuntimeChanged(EtchRuntime* runtime) {
    mId++;
    return ETCH_OK;
  }
public:
  capu::uint8_t mId;
};

TEST(EtchRuntime, registerListener) {
  status_t status;
  EtchRuntime* runtime = new EtchRuntime();

  status = runtime->registerListener(NULL);
  EXPECT_EQ(ETCH_EINVAL, status);

  EtchRuntimeTestListener listener;
  status = runtime->registerListener(&listener);
  EXPECT_EQ(ETCH_OK, status);

  // a shutdown leads to a runtime change
  runtime->shutdown();

  status = runtime->unregisterListener(&listener);
  EXPECT_EQ(ETCH_OK, status);

  // check if callback was fired
  EXPECT_EQ(1, listener.mId);

  delete runtime;
}
