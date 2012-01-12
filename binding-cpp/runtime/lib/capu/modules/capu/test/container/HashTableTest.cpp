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
#include "capu/container/HashTable.h"
#include "capu/Error.h"

TEST(HashTable, Constructor_Default) {
  //create an empty linked list
  capu::HashTable<capu::int32_t, capu::int32_t>* list = new capu::HashTable<capu::int32_t, capu::int32_t > ();
  delete list;
}

TEST(HashTable, put) {
  capu::int32_t key = 10;
  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;
  capu::int64_t count = -1;

  capu::HashTable<capu::int32_t, capu::int32_t>* h1 = new capu::HashTable<capu::int32_t, capu::int32_t > ();
  // add new key
  status = h1->put(key, value);
  EXPECT_TRUE(status == capu::CAPU_OK);
  // check count
  count = h1->count();
  EXPECT_TRUE(count == 1);
  delete h1;
}

TEST(HashTable, count) {
  capu::int64_t count = -1;

  capu::HashTable<capu::int32_t, capu::int32_t>* h1 = new capu::HashTable<capu::int32_t, capu::int32_t > ();
  //check count
  count = h1->count();
  EXPECT_TRUE(count == 0);

  delete h1;
}

TEST(HashTable, get) {
  capu::int32_t key = 5;
  capu::int32_t key2 = 6;
  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;

  capu::int32_t return_value = -1;
  capu::int64_t count = -1;

  capu::HashTable<capu::int32_t, capu::int32_t>* h1 =
          new capu::HashTable<capu::int32_t, capu::int32_t > ();
  // add new key
  status = h1->put(key, value);
  EXPECT_TRUE(status == capu::CAPU_OK);
  // check count
  count = h1->count();
  EXPECT_TRUE(count == 1);

  // get the added element
  status = h1->get(key, &return_value);
  EXPECT_TRUE(status == capu::CAPU_OK);
  //check its value
  EXPECT_TRUE(return_value == value);
  //get a element to null variable
  status = h1->get(key, NULL);
  EXPECT_TRUE(status == capu::CAPU_EINVAL);
  //get an element of non existing key
  status = h1->get(key2, &return_value);
  EXPECT_TRUE(status == capu::CAPU_ENOT_EXIST);

  delete h1;
}

TEST(HashTable, clear) {
  capu::int32_t key = 5;
  capu::int32_t key2 = 6;
  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;

  capu::int64_t count = -1;

  capu::HashTable<capu::int32_t, capu::int32_t>* h1 = new capu::HashTable<capu::int32_t, capu::int32_t > ();
  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //add new keys
  status = h1->put(key2, value);
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

TEST(HashTable, remove) {
  capu::int32_t key = 5;
  capu::int32_t key2 = 6;
  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;

  capu::int32_t return_value = -1;
  capu::int64_t count = -1;

  capu::HashTable<capu::int32_t, capu::int32_t>* h1 = new capu::HashTable<capu::int32_t, capu::int32_t > ();
  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == capu::CAPU_OK);
  //delete a non existing key
  status = h1->remove(key2, &return_value);
  EXPECT_TRUE(status == capu::CAPU_ERANGE);

  //add new value
  status = h1->put(key2, value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 2);

  //delete existing key
  status = h1->remove(key, &return_value);
  EXPECT_TRUE(status == capu::CAPU_OK);
  EXPECT_TRUE(value == return_value);

  //check count
  count = h1->count();
  EXPECT_TRUE(count == 1);

  delete h1;
}

TEST(HashTable, set_get_existing) {
  capu::int32_t key = 10;

  capu::int32_t key2 = 11;
  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;

  capu::int32_t return_value = -1;
  capu::int64_t count = -1;

  capu::HashTable<capu::int32_t, capu::int32_t>* h1 = new capu::HashTable<capu::int32_t, capu::int32_t > ();
  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //add new value
  status = h1->put(key2, value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  value = 3;
  //add new value over existing one
  status = h1->put(key, value, &return_value);
  EXPECT_TRUE(status == capu::CAPU_OK);
  //check the retrieved old value
  EXPECT_TRUE(5 == return_value);

  //check the new value
  status = h1->get(key, &return_value);
  EXPECT_TRUE(3 == return_value);

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 2);

  delete h1;

}

TEST(HashtableIterator, hasNext) {
  capu::int32_t key = 10;
  capu::int32_t key2 = 12;

  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;

  capu::HashTable<capu::int32_t, capu::int32_t>* h1 = new capu::HashTable<capu::int32_t, capu::int32_t > ();

  //create iterator
  capu::HashTable<capu::int32_t, capu::int32_t>::Iterator it = h1->begin();
  //check hasNext
  EXPECT_TRUE(it.hasNext() == false);

  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //add new value
  status = h1->put(key2, value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  it = h1->begin();
  EXPECT_TRUE(it.hasNext() == true);

  delete h1;
}

TEST(HashtableIterator, next) {
  capu::int32_t key = 10;
  capu::int32_t key2 = 12;

  capu::int32_t value = 5;
  capu::status_t status = capu::CAPU_OK;
  capu::Pair<capu::int32_t, capu::int32_t> pair;
  capu::HashTable<capu::int32_t, capu::int32_t>* h1 = new capu::HashTable<capu::int32_t, capu::int32_t > ();

  //create iterator
  capu::HashTable<capu::int32_t, capu::int32_t>::Iterator it = h1->begin();

  //check hasNext
  EXPECT_TRUE(it.hasNext() == false);

  EXPECT_TRUE(it.next(&pair) == capu::CAPU_ERANGE);

  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == capu::CAPU_OK);

  //add new value
  status = h1->put(key2, value);
  EXPECT_TRUE(status == capu::CAPU_OK);
  it = h1->begin();

  it.next(&pair);
  EXPECT_TRUE(pair.first == key);
  EXPECT_TRUE(pair.second == value);

  it.next(&pair);
  EXPECT_TRUE(pair.first == key2);
  EXPECT_TRUE(pair.second == value);
  delete h1;
}
