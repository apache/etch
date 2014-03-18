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
#include "common/EtchHashTable.h"
#include "common/EtchComparatorNative.h"
#include "common/EtchHashNative.h"
#include "common/EtchInt32.h"
#include "common/EtchString.h"

TEST(EtchHashTableTest, Constructor_Default){
  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();
  delete h1;

  EtchHashTable<char*, capu::int32_t, EtchComparatorNative, EtchHashNative >* h2 = new EtchHashTable<char*, capu::int32_t, EtchComparatorNative, EtchHashNative > ();
  delete h2;
}

TEST(EtchHashTableTest, put){
  EtchString key("key1");
  EtchInt32 value(5);
  status_t status = ETCH_OK;
  capu::uint64_t count = 5;

  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();
  // add new key
  status = h1->put(key, value);
  EXPECT_TRUE(status == ETCH_OK);
  // check count
  count = h1->count();
  EXPECT_TRUE(count == 1);

  EtchHashTable<char*, capu::int32_t, EtchComparatorNative, EtchHashNative >* h2 = new EtchHashTable<char*, capu::int32_t, EtchComparatorNative, EtchHashNative> ();
  // add new key
  char* key1 = const_cast<char*>("key1");
  capu::int32_t value1 = 5;
  status = h2->put(key1, value1);
  EXPECT_TRUE(status == ETCH_OK);
  // check count
  count = h2->count();
  EXPECT_TRUE(count == 1);

  delete h1;
  delete h2;
}

TEST(EtchHashTableTest, count){
  EtchString key("key");
  EtchInt32 value(5);
  capu::uint64_t count = 5;
  status_t status = ETCH_OK;

  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();
  //check count
  count = h1->count();
  EXPECT_TRUE(count == 0);

  //add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == ETCH_OK);

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 1);

  delete h1;
}

TEST(EtchHashTableTest, get){
  EtchString key("key");
  EtchString key2("key2");
  EtchInt32 value(5);
  status_t status = ETCH_OK;

  EtchInt32 return_value(-1);
  capu::uint64_t count = 5;

  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();
  // add new key
  status = h1->put(key, value);
  EXPECT_TRUE(status == ETCH_OK);
  // check count
  count = h1->count();
  EXPECT_TRUE(count == 1);

  // get the added element
  status = h1->get(key, &return_value);
  EXPECT_TRUE(status == ETCH_OK);
  //check its value
  EXPECT_TRUE(return_value.get() == value.get());
  //get a element to null variable
  status = h1->get(key, NULL);
  EXPECT_TRUE(status == ETCH_EINVAL);
  //get an element of non existing key
  status = h1->get(key2, &return_value);
  EXPECT_TRUE(status == ETCH_ENOT_EXIST);

  EtchHashTable<char*, capu::int32_t, EtchComparatorNative, EtchHashNative >* h2 = new EtchHashTable<char*, capu::int32_t, EtchComparatorNative, EtchHashNative > ();
  // add new key
  char* key1 = const_cast<char*>("key1");
  capu::int32_t value1 = 5;
  capu::int32_t return_value1;
  status = h2->put(key1, value1);
  EXPECT_TRUE(status == ETCH_OK);
  // get the added element
  status = h2->get(key1, &return_value1);
  EXPECT_TRUE(status == ETCH_OK);
  //check its value
  EXPECT_TRUE(return_value1 == value1);

  delete h1;
  delete h2;
}

TEST(EtchHashTableTest, clear){
  EtchString key("key");
  EtchString key2("key2");
  EtchInt32 value(5);
  status_t status = ETCH_OK;

  capu::uint64_t count = 5;

  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();
  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == ETCH_OK);

  //add new keys
  status = h1->put(key2, value);
  EXPECT_TRUE(status == ETCH_OK);

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 2);

  //remove all
  status = h1->clear();
  EXPECT_TRUE(status == ETCH_OK);

  //check count
  count = h1->count();
  EXPECT_TRUE(count == 0);

  delete h1;
}

TEST(EtchHashTableTest, remove){
  EtchString key("key");
  EtchString key2("key2");
  EtchInt32 value(5);
  status_t status = ETCH_OK;

  EtchInt32 return_value(-1);
  capu::uint64_t count = 5;

  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();
  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == ETCH_OK);
  //delete a non existing key
  status = h1->remove(key2, &return_value);
  EXPECT_TRUE(status == ETCH_ERANGE);

  //add new value
  status = h1->put(key2, value);
  EXPECT_TRUE(status == ETCH_OK);

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 2);

  //delete existing key
  status = h1->remove(key, &return_value);
  EXPECT_TRUE(status == ETCH_OK);
  EXPECT_TRUE(value.get() == return_value.get());

  //check count
  count = h1->count();
  EXPECT_TRUE(count == 1);

  delete h1;
}

TEST(EtchHashTableTest, put_get_existing){
  EtchString key("key");
  EtchString key2("key2");
  EtchInt32 value(5);
  status_t status = ETCH_OK;

  EtchInt32 return_value(-1);
  capu::uint64_t count = 5;

  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();
  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == ETCH_OK);

  //add new value
  status = h1->put(key2, value);
  EXPECT_TRUE(status == ETCH_OK);

  value.set(3);
  //add new value over existing one
  status = h1->put(key, value, &return_value);
  EXPECT_TRUE(status == ETCH_OK);
  //check the retrieved old value
  EXPECT_TRUE(5 == return_value.get());

  //check the new value
  status = h1->get(key, &return_value);
  EXPECT_TRUE(3 == return_value.get());

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 2);

  delete h1;

}

TEST(EtchHashTableIterator, hasNext){
  EtchString key("key");
  EtchString key2("key2");
  EtchInt32 value(5);
  status_t status = ETCH_OK;

  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();

  //create iterator
  EtchHashTable<EtchString, EtchInt32>::Iterator it = h1->begin();
  //check hasNext
  EXPECT_TRUE(it.hasNext() == false);

  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == ETCH_OK);

  //add new value
  status = h1->put(key2, value);
  EXPECT_TRUE(status == ETCH_OK);

  it = h1->begin();
  EXPECT_TRUE(it.hasNext() == true);

  delete h1;
}

TEST(EtchHashTableConstIterator, hasNext){
	EtchString key("key");
	EtchString key2("key2");
	EtchInt32 value(5);
	status_t status = ETCH_OK;

	EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();

	//create iterator
	EtchHashTable<EtchString, EtchInt32>::Iterator it = h1->begin();
	//check hasNext
	EXPECT_TRUE(it.hasNext() == false);

	// add new keys
	status = h1->put(key, value);
	EXPECT_TRUE(status == ETCH_OK);

	//add new value
	status = h1->put(key2, value);
	EXPECT_TRUE(status == ETCH_OK);

	it = h1->begin();
	EXPECT_TRUE(it.hasNext() == true);

	delete h1;
}

TEST(EtchHashTableIterator, next){
  EtchString key("key");
  EtchString key2("key2");
  EtchInt32 value(5);
  status_t status = ETCH_OK;

  EtchHashTable<EtchString, EtchInt32>* h1 = new EtchHashTable<EtchString, EtchInt32 > ();

  //create iterator
  EtchHashTable<EtchString, EtchInt32>::Iterator it = h1->begin();
  //check hasNext
  EXPECT_TRUE(it.hasNext() == false);
  EtchHashTable<EtchString, EtchInt32>::HashTableEntry entry;
  EXPECT_TRUE(it.next(&entry) == ETCH_ERANGE);

  // add new keys
  status = h1->put(key, value);
  EXPECT_TRUE(status == ETCH_OK);

  //add new value
  status = h1->put(key2, value);
  EXPECT_TRUE(status == ETCH_OK);

  it = h1->begin();

  EXPECT_EQ(ETCH_OK, it.next(&entry));
  EXPECT_EQ(ETCH_OK, it.next(&entry));
  EXPECT_EQ(ETCH_ERANGE, it.next(&entry));

  delete h1;
}
