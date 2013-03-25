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

#include "common/EtchError.h"
#include "common/EtchHashSet.h"
#include "common/EtchComparatorNative.h"
#include "common/EtchHashNative.h"
#include "common/EtchString.h"

TEST(EtchHashSet, Constructor_Default){
  EtchHashSet<EtchString>* set = new EtchHashSet<EtchString > ();
  delete set;

  EtchHashSet<char*, EtchComparatorNative, EtchHashNative >* set2 = new EtchHashSet<char*, EtchComparatorNative, EtchHashNative > ();
  delete set2;
}

TEST(EtchHashSet, put){
  EtchString value2("value2");
  EtchString value("value");
  status_t status = ETCH_OK;

  EtchHashSet<EtchString>* h1 = new EtchHashSet<EtchString> ();

  // add new key
  status = h1->put(value);
  EXPECT_TRUE(status == ETCH_OK);

  // add new key
  status = h1->put(value2);
  EXPECT_TRUE(status == ETCH_OK);

  // add existing key
  status = h1->put(value2);
  EXPECT_TRUE(status == ETCH_ERROR);

  EtchHashSet<char*, EtchComparatorNative, EtchHashNative >* h2 = new EtchHashSet<char*, EtchComparatorNative, EtchHashNative > ();

  // add new key
  char* value3 = const_cast<char*>("val1");
  char* value4 = const_cast<char*>("val2");
  status = h2->put(value3);
  EXPECT_TRUE(status == ETCH_OK);

  // add new key
  status = h2->put(value4);
  EXPECT_TRUE(status == ETCH_OK);

  // add existing key
  status = h2->put(value4);
  EXPECT_TRUE(status == ETCH_ERROR);

  delete h1;
  delete h2;
}

TEST(EtchHashSet, count){
  capu::uint64_t count = 5;
  EtchString value2("value2");
  EtchString value("value");
  status_t status = ETCH_OK;
  EtchHashSet<EtchString>* h1 = new EtchHashSet<EtchString> ();

  //check count
  count = h1->count();
  EXPECT_TRUE(count == 0);

  // add new value
  status = h1->put(value);
  EXPECT_TRUE(status == ETCH_OK);

  // add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == ETCH_OK);

  delete h1;
}

TEST(EtchHashSet, clear){
  EtchString value2("value2");
  EtchString value("value");
  status_t status = ETCH_OK;

  capu::uint64_t count = 5;

  EtchHashSet<EtchString>* h1 = new EtchHashSet<EtchString> ();
  // add new keys
  status = h1->put(value);
  EXPECT_TRUE(status == ETCH_OK);

  //add new keys
  status = h1->put(value2);
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

TEST(EtchHashSet, remove){
  EtchString value2("value2");
  EtchString value("value");
  status_t status = ETCH_OK;

  capu::uint64_t count = 5;

  EtchHashSet<EtchString>* h1 = new EtchHashSet<EtchString> ();
  // add new keys
  status = h1->put(value);
  EXPECT_TRUE(status == ETCH_OK);
  //delete a non existing value
  status = h1->remove(value2);
  EXPECT_TRUE(status == ETCH_ERANGE);

  //add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == ETCH_OK);

  // check count
  count = h1->count();
  EXPECT_TRUE(count == 2);

  //delete existing value
  status = h1->remove(value2);
  EXPECT_TRUE(status == ETCH_OK);

  //check count
  count = h1->count();
  EXPECT_TRUE(count == 1);

  delete h1;
}

TEST(EtchHashSetIterator, hasNext){
  EtchString value2("value2");
  EtchString value("value");

  status_t status = ETCH_OK;

  EtchHashSet<EtchString>* h1 = new EtchHashSet<EtchString> ();

  //create iterator
  EtchHashSet<EtchString>::Iterator it = h1->begin();
  //check hasNext
  EXPECT_TRUE(it.hasNext() == false);

  // add new values
  status = h1->put(value);
  EXPECT_TRUE(status == ETCH_OK);

  //add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == ETCH_OK);

  it = h1->begin();
  EXPECT_TRUE(it.hasNext() == true);

  delete h1;
}

TEST(EtchHashSetIterator, NEXT){
  EtchString value2("value2");
  EtchString value("value");

  status_t status = ETCH_OK;
  EtchHashSet<EtchString>* h1 = new EtchHashSet<EtchString> ();

  EtchString check_value = 0;
  //create iterator
  EtchHashSet<EtchString>::Iterator it = h1->begin();

  //check hasNext
  EXPECT_TRUE(it.hasNext() == false);

  EXPECT_TRUE(it.next(&check_value) == ETCH_ERANGE);

  // add new keys
  status = h1->put(value);
  EXPECT_TRUE(status == ETCH_OK);

  //add new value
  status = h1->put(value2);
  EXPECT_TRUE(status == ETCH_OK);
  it = h1->begin();

  it.next(&check_value);
  it.next(&check_value);
  EXPECT_EQ(ETCH_ERANGE,it.next());

  delete h1;
}
