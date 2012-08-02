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

TEST(EtchRuntime, Default) {
  EtchRuntime* runtime = new EtchRuntime();
  delete runtime;
}

TEST(EtchRuntime, getId) {
  EtchRuntime* runtime = new EtchRuntime();
  capu::uint64_t id = runtime->getId();
  delete runtime;
}

TEST(EtchRuntime, isClosed) {
  EtchRuntime* runtime = new EtchRuntime();
  ASSERT_EQ(false, runtime->isClosed());
  runtime->shutdown();
  ASSERT_EQ(true, runtime->isClosed());
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
  ASSERT_EQ(ETCH_EINVAL, status);

  EtchRuntimeTestListener listener;
  status = runtime->registerListener(&listener);
  ASSERT_EQ(ETCH_OK, status);

  // a shutdown leads to a runtime change
  runtime->shutdown();

  status = runtime->unregisterListener(&listener);
  ASSERT_EQ(ETCH_OK, status);

  // check if callback was fired
  ASSERT_EQ(1, listener.mId);

  delete runtime;
}
