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
#include "capu/container/HashSet.h"
#include "capu/Error.h"
#include "capu/Config.h"

TEST(HashSet, Constructor_Default) {
  capu::HashSet<capu::int32_t>* list = new capu::HashSet<capu::int32_t > ();
  delete list;
}

TEST(HashSet, put) {
  capu::int32_t value2 = 10;
  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;

  capu::HashSet<capu::int32_t>* h1 = new capu::HashSet<capu::int32_t > ();

  // add new key
  status = h1->put(value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // add new key
  status = h1->put(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // add existing key
  status = h1->put(value2);
  EXPECT_TRUE(status == capu::CAPU_ERROR);

  delete h1;
}

TEST(HashSet, count) {
  capu::uint64_t count = 0;
  capu::int32_t value2 = 10;
  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;
  capu::HashSet<capu::int32_t>* h1 = new capu::HashSet<capu::int32_t > ();

  //check count
  count = h1->count();
  EXPECT_TRUE(count == 0);

  // add new value
  status = h1->put(value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);

  count = h1->count();
  EXPECT_TRUE(count == 2);

  status = h1->remove(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);

  count = h1->count();
  EXPECT_TRUE(count == 1);

  delete h1;
}

TEST(HashSet, clear) {
  capu::int32_t value = 5;
  capu::int32_t value2 = 6;
  capu::status_t status = capu::CAPU_OK;

  capu::uint64_t count = 0;

  capu::HashSet<capu::int32_t>* h1 = new capu::HashSet<capu::int32_t > ();
  // add new keys
  status = h1->put(value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //add new keys
  status = h1->put(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 2);

  //remove all
  status = h1->clear();
  EXPECT_TRUE(status == capu::CAPU_OK);

  //check count
  count = h1->count();
  EXPECT_TRUE(count == 0);

  delete h1;
}

TEST(HashSet, remove) {
  capu::int32_t value = 5;
  capu::int32_t value2 = 6;
  capu::status_t status = capu::CAPU_OK;

  capu::uint64_t count = 0;

  capu::HashSet<capu::int32_t>* h1 = new capu::HashSet<capu::int32_t > ();
  // add new keys
  status = h1->put(value);
  EXPECT_TRUE(status == capu::CAPU_OK);
  //delete a non existing value
  status = h1->remove(value2);
  EXPECT_TRUE(status == capu::CAPU_ERANGE);

  //add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 2);

  //delete existing value
  status = h1->remove(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //check count
  count = h1->count();
  EXPECT_TRUE(count == 1);

  delete h1;
}

TEST(HashSet, hasElement) {
  capu::int32_t value = 5;
  capu::int32_t value2 = 6;
  capu::status_t status = capu::CAPU_OK;
  capu::bool_t isElementinHashSet;

  capu::HashSet<capu::int32_t>* h1 = new capu::HashSet<capu::int32_t > ();
  // add new keys
  status = h1->put(value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  isElementinHashSet = h1->hasElement(value2);
  EXPECT_TRUE(isElementinHashSet == false);

  //add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //delete existing value
  isElementinHashSet = h1->hasElement(value2);
  EXPECT_TRUE(isElementinHashSet == true);

  delete h1;
}

TEST(HashSetIterator, hasNext) {
  capu::int32_t value = 10;
  capu::int32_t value2 = 12;

  capu::status_t status = capu::CAPU_OK;

  capu::HashSet<capu::int32_t>* h1 = new capu::HashSet<capu::int32_t > ();

  //create iterator
  capu::HashSet<capu::int32_t>::Iterator it = h1->begin();
  //check hasNext
  EXPECT_TRUE(it.hasNext() == false);

  // add new values
  status = h1->put(value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);

  it = h1->begin();
  EXPECT_TRUE(it.hasNext() == true);

  delete h1;
}

TEST(HashSetIterator, next) {
  capu::int32_t value = 10;
  capu::int32_t value2 = 12;

  capu::status_t status = capu::CAPU_OK;
  capu::HashSet<capu::int32_t>* h1 = new capu::HashSet<capu::int32_t > ();

  capu::int32_t check_value = 0;
  capu::int32_t check_value2 = 0;
  //create iterator
  capu::HashSet<capu::int32_t>::Iterator it = h1->begin();

  //check hasNext
  EXPECT_TRUE(it.hasNext() == false);

  EXPECT_TRUE(it.next(&check_value) == capu::CAPU_ERANGE);

  // add new keys
  status = h1->put(value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == capu::CAPU_OK);
  it = h1->begin();

  it.next(&check_value);
  EXPECT_TRUE(check_value == value || check_value == value2);

  it.next(&check_value2);
  EXPECT_TRUE(check_value == value || check_value == value2);

  EXPECT_FALSE(check_value == check_value2);
  delete h1;
}
