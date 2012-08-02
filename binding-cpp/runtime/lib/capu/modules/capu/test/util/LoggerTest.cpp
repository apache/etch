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
#include "capu/util/Logger.h"
#include "capu/util/Appender.h"

class DummyAppender : public capu::Appender {
  public:
    class LoggerMessage;

    capu::status_t open() {
      return capu::CAPU_OK;
    }

    capu::status_t log(capu::LoggerMessage* message) {
      //printf("[%d] %s:%d\n", message->getTimestamp(), message->getFile(), message->getLine());
      return capu::CAPU_OK;
    }

    capu::status_t close() {
      return capu::CAPU_OK;
    }

};

TEST(Logger, Constructor_Default) {
  capu::Logger* log1 = new capu::Logger();
  EXPECT_TRUE(log1 != NULL);
  delete log1;
}

TEST(Logger, setAppender) {
  capu::Logger* log1 = new capu::Logger();
  EXPECT_TRUE(log1 != NULL);

  DummyAppender* appenders[11];
  for(int i = 0; i < LOGGER_APPENDER_MAX + 1; i++) {
     appenders[i] = new DummyAppender();
  }

  for(int i = 0; i < LOGGER_APPENDER_MAX; i++) {
    capu::status_t status = log1->setAppender(appenders[i]);
    EXPECT_TRUE(status == capu::CAPU_OK);
  }

  capu::status_t status = log1->setAppender(appenders[LOGGER_APPENDER_MAX]);
  EXPECT_TRUE(status == capu::CAPU_ERROR);

  log1->open();
  log1->close();

  for(int i = 0; i < LOGGER_APPENDER_MAX + 1; i++) {
     delete appenders[i];
  }

  delete log1;
}

TEST(Logger, log) {
  capu::status_t status;

  capu::Logger* l1 = new capu::Logger();
  EXPECT_TRUE(l1 != NULL);

  // set appender
  DummyAppender* a1 = new DummyAppender();
  status = l1->setAppender(a1);
  EXPECT_TRUE(status == capu::CAPU_OK);

  const char* TAG = "DummyTAG";

  // trace
  status = l1->trace(TAG, __FILE__, __LINE__, "Log trace message test");
  EXPECT_TRUE(status == capu::CAPU_OK);
  status = l1->trace(TAG, __FILE__, __LINE__, "Log trace message test with param %d", 1234);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // debug
  status = l1->debug(TAG, __FILE__, __LINE__, "Log debug message test");
  EXPECT_TRUE(status == capu::CAPU_OK);
  status = l1->debug(TAG, __FILE__, __LINE__, "Log debug message test with param %d", 1234);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // info
  status = l1->info(TAG, __FILE__, __LINE__, "Log info message test");
  EXPECT_TRUE(status == capu::CAPU_OK);
  status = l1->info(TAG, __FILE__, __LINE__, "Log info message test with param %d", 1234);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // warn
  status = l1->warn(TAG, __FILE__, __LINE__, "Log warn message test");
  EXPECT_TRUE(status == capu::CAPU_OK);
  status = l1->warn(TAG, __FILE__, __LINE__, "Log warn message test with param %d", 1234);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // error
  status = l1->error(TAG, __FILE__, __LINE__, "Log error message test");
  EXPECT_TRUE(status == capu::CAPU_OK);
  status = l1->error(TAG, __FILE__, __LINE__, "Log error message test with param %d", 1234);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // level
  status = l1->log(capu::CLL_ERROR, TAG, __FILE__, __LINE__, "Log error message test");
  EXPECT_TRUE(status == capu::CAPU_OK);
  status = l1->log(capu::CLL_ERROR, TAG, __FILE__, __LINE__, "Log error message test with param %d", 1234);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // check log macros
  CAPU_LOG(l1, capu::CLL_ERROR, TAG, "Log error message test");
  CAPU_LOG(l1, capu::CLL_ERROR, TAG, "Log error message test with param %d", 123);
  CAPU_LOG_TRACE(l1, TAG, "Log error message test");
  CAPU_LOG_TRACE(l1, TAG, "Log error message test with param %d", 123);
  CAPU_LOG_DEBUG(l1, TAG, "Log error message test");
  CAPU_LOG_DEBUG(l1, TAG, "Log error message test with param %d", 123);
  CAPU_LOG_INFO(l1, TAG, "Log error message test");
  CAPU_LOG_INFO(l1, TAG, "Log error message test with param %d", 123);
  CAPU_LOG_WARN(l1, TAG, "Log error message test");
  CAPU_LOG_WARN(l1, TAG, "Log error message test with param %d", 123);
  CAPU_LOG_ERROR(l1, TAG, "Log error message test");
  CAPU_LOG_ERROR(l1, TAG, "Log error message test with param %d", 123);

  // open logger
  status = l1->open();
  EXPECT_TRUE(status == capu::CAPU_OK);

  // close logger
  status = l1->close();
  EXPECT_TRUE(status == capu::CAPU_OK);

  delete a1;
  delete l1;
}

