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
#include "support/EtchAsyncResult.h"

TEST(EtchAsyncResultTest, constructor) {
  EtchAsyncResultNone* r1 = new EtchAsyncResultNone(NULL, NULL);
  delete r1;

  EtchAsyncResult<EtchObject>* r2 = new EtchAsyncResult<EtchObject>(NULL, NULL);
  delete r2;
}

TEST(EtchAsyncResultTest, hasException) {
  EtchAsyncResultNone* r1 = new EtchAsyncResultNone(NULL, NULL);
  // TODO test with real mailbox and a thread
  delete r1;
}

TEST(EtchAsyncResultTest, hasResult) {
  EtchAsyncResult<EtchObject>* r1 = new EtchAsyncResult<EtchObject>(NULL, NULL);
  // TODO test with real mailbox and a thread
  delete r1;
}
