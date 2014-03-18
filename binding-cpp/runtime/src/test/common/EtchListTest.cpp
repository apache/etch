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
#include "common/EtchInt32.h"
#include "common/EtchList.h"
#include "common/EtchString.h"

TEST(EtchList, Constructor_Default) {
  //create an empty linked list
  EtchList<EtchInt32*>* list = new EtchList<EtchInt32*>();
  delete list;
}

TEST(EtchList, addTest) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1;
  EtchInt32 data2;
  EtchInt32 data3;
  status_t result;

  data1.set(32);
  data2.set(43);
  //append element to the linked list
  result = list->add(data1);
  EXPECT_TRUE(result == ETCH_OK);

  //append another element to linked list
  result = list->add(data2);
  EXPECT_TRUE(result == ETCH_OK);

  //Get added elements to compare that if they are correctly added or not
  list->get(0, &data3);
  EXPECT_TRUE(data3.get() == data1.get());

  list->get(1, &data3);
  EXPECT_TRUE(data3.get() == data2.get());

  delete list;
}

TEST(EtchList, removeAt) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1;
  EtchInt32 data2;
  EtchInt32 data3;
  EtchInt32 data4;
  status_t result;

  data1.set(32);
  data2.set(43);
  data3.set(56);
  //add some elements to linked list
  result = list->add(data1);
  EXPECT_TRUE(result == ETCH_OK);

  result = list->add(data2);
  EXPECT_TRUE(result == ETCH_OK);

  result = list->add(data3);
  EXPECT_TRUE(result == ETCH_OK);

  EtchList<EtchInt32>::Iterator it = list->begin();

  result = list->removeAt(it, &data4);
  EXPECT_TRUE(result == ETCH_OK);
  EXPECT_TRUE(data4.get() == data1.get());

  result = list->removeAt(it, &data4);
  EXPECT_TRUE(result == ETCH_OK);
  EXPECT_TRUE(data4.get() == data2.get());

  result = list->removeAt(it, &data4);
  EXPECT_TRUE(result == ETCH_OK);
  EXPECT_TRUE(data4.get() == data3.get());
  
  EXPECT_FALSE(it.hasNext());
  EXPECT_EQ(ETCH_ERROR, it.next());

  //check size of list
  EXPECT_TRUE(list->size() == 0);

  delete list;
}

TEST(EtchList, get) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1;
  EtchInt32 data2;
  EtchInt32 data3;
  EtchInt32 data4;

  status_t result;

  data1.set(32);
  data2.set(43);
  data3.set(56);

  //add some element to the linked list
  result = list->add(data1);
  EXPECT_TRUE(result == ETCH_OK);

  result = list->add(data3);
  EXPECT_TRUE(result == ETCH_OK);

  result = list->add(data2);
  EXPECT_TRUE(result == ETCH_OK);

  //get the added elements by using its index and compare the values with inserted elements
  result = list->get(0, &data4);
  EXPECT_TRUE(result == ETCH_OK);
  EXPECT_TRUE(data1.get() == data4.get());

  result = list->get(1, &data4);
  EXPECT_TRUE(result == ETCH_OK);
  EXPECT_TRUE(data3.get() == data4.get());

  result = list->get(2, &data4);
  EXPECT_TRUE(result == ETCH_OK);
  EXPECT_TRUE(data2.get() == data4.get());

  result = list->get(123, NULL);
  EXPECT_TRUE(result == ETCH_EINVAL);

  delete list;
}

TEST(EtchList, size) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1;
  EtchInt32 data2;
  EtchInt32 data3;
  EtchInt32 data4;

  capu::int32_t result;

  data1.set(32);
  data2.set(43);
  data3.set(56);

  //size of empty list
  result = list->size();
  EXPECT_TRUE(result == 0);

  //add some elements and check the size for each step
  list->add(data1);
  result = list->size();
  EXPECT_TRUE(result == 1);

  list->add(data1);
  result = list->size();
  EXPECT_TRUE(result == 2);

  list->add(data1);
  result = list->size();
  EXPECT_TRUE(result == 3);

  delete list;
}

TEST(EtchList, empty) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1;
  EtchInt32 data2;
  EtchInt32 data3;
  EtchInt32 data4;

  capu::bool_t result;

  data1.set(32);
  data2.set(43);
  data3.set(56);

  //check the empty list
  result = list->isEmpty();
  EXPECT_TRUE(result == true);

  //add some element
  list->add(data1);
  result = list->isEmpty();
  EXPECT_TRUE(result == false);

  delete list;
}

TEST(EtchList, set) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1(32);
  EtchInt32 data2(43);

  EXPECT_TRUE(list->add(data1) == ETCH_OK);
  EXPECT_TRUE(list->add(data2) == ETCH_OK);
  EXPECT_TRUE(list->set(1, data1) == ETCH_OK);
  EXPECT_TRUE(list->set(0, data1) == ETCH_OK);
  EXPECT_TRUE(list->get(1, &data2) == ETCH_OK);
  EXPECT_TRUE(list->set(2, data1) == ETCH_EINVAL);
  EXPECT_TRUE(data2.get() == data1.get());
  delete list;
}

TEST(EtchList, find) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1(32);
  EtchInt32 data2(43);
  EtchInt32 data3(44);
  capu::int32_t result;

  //add some elements
  list->add(data1);
  list->add(data2);
  list->add(data3);

  //find the elements
  result = list->find(data1).currentIndex();
  EXPECT_EQ(0, result);

  result = list->find(data2).currentIndex();
  EXPECT_EQ(1, result);

  result = list->find(data3).currentIndex();
  EXPECT_EQ(2, result);

  delete list;
}

TEST(EtchList, contains) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1(32);
  EtchInt32 data2(43);
  EtchInt32 data3(44);

  EtchInt32 check_value(0);
  capu::bool_t result;

  // fill the linked
  list->add(data1);
  list->add(data1);
  list->add(data2);
  list->add(data3);

  //check an elements to be contained by linked list or not
  result = list->contains(check_value);
  EXPECT_TRUE(result == false);

  result = list->contains(data3);
  EXPECT_TRUE(result == true);

  delete list;
}

TEST(EtchList, clear) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1(32);
  EtchInt32 data2(43);
  EtchInt32 data3(44);
  status_t result;

  //add some dummy values to the linked list
  list->add(data1);
  list->add(data1);
  list->add(data2);
  list->add(data3);

  //remove all elements from the linked list
  result = list->clear();
  EXPECT_TRUE(result == ETCH_OK);

  result = list->add(data1);
  EXPECT_TRUE(result == ETCH_OK);

  result = list->get(0, &data2);
  EXPECT_TRUE(data1.get() == data2.get());
  delete list;
}

TEST(EtchList, addTestIndex) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1(32);
  EtchInt32 data2(43);
  EtchInt32 data3(44);
  EtchInt32 new_value(5);

  EtchInt32 check_value(0);
  status_t result;

  // add some dummy elements to linked list
  list->add(data1);
  list->add(data1);
  list->add(data2);
  list->add(data3);

  //before the replacing the element
  result = list->get(1, &check_value);
  EXPECT_TRUE(check_value.get() == data1.get());

  //replace the element
  result = list->add(1, new_value);
  EXPECT_TRUE(result == ETCH_OK);

  //after the replacing the element, check the values
  result = list->get(1, &check_value);
  EXPECT_TRUE(check_value.get() == new_value.get());

  result = list->get(0, &check_value);
  EXPECT_TRUE(check_value.get() == data1.get());

  delete list;
}

TEST(EtchListIterator, hasNext) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1;
  EtchInt32 data2;
  EtchList<EtchInt32>::Iterator it = list->begin();
  EXPECT_TRUE(it.hasNext() == false);

  data1.set(32);
  data2.set(43);

  it = list->begin();
  capu::bool_t resultb = it.hasNext();
  EXPECT_TRUE(resultb == false);

  list->add(data1);
  it = list->begin();

  resultb = it.hasNext();
  EXPECT_TRUE(resultb == true);
  delete list;
}

TEST(EtchListIterator, next) {
  EtchList<EtchInt32>* list = new EtchList<EtchInt32 > ();
  EtchInt32 data1;
  EtchInt32 data2;
  EtchInt32 data3;
  EtchList<EtchInt32>::Iterator it = list->begin();
  capu::int32_t cnt = 0;

  EXPECT_TRUE(it.hasNext() == false);

  data1.set(32);
  data2.set(43);
  list->add(data1);
  list->add(data2);
  it = list->begin();
  while (it.hasNext()) {
    it.next(&data3);
    if (cnt == 0)
      EXPECT_TRUE(data3.get() == data1.get());
    else
      EXPECT_TRUE(data3.get() == data2.get());
    cnt++;
  }
  delete list;
}

TEST(EtchListConstIterator, hasNext) {

	EtchList<EtchInt32>* normalList = new EtchList<EtchInt32 > ();
	
	EtchInt32 data1;
	EtchInt32 data2;
	data1.set(32);
	data2.set(43);

	const EtchList<EtchInt32>* constList = normalList;
	EtchList<EtchInt32>::ConstIterator it = constList->begin();
	EXPECT_FALSE(it.hasNext());

	normalList->add(data1);

	it = constList->begin();

	EXPECT_TRUE(it.hasNext());
	
	delete normalList;
}

TEST(EtchListConstIterator, next) {
	EtchList<EtchInt32>* normalList = new EtchList<EtchInt32 > ();
	
	EtchInt32 data1;
	EtchInt32 data2;
	EtchInt32 data3;

	data1.set(32);
	data2.set(43);
	normalList->add(data1);
	normalList->add(data2);

	const EtchList<EtchInt32>* constList = normalList;
	
	EtchList<EtchInt32>::ConstIterator it = constList->begin();
	capu::int32_t cnt = 0;

	while (it.hasNext()) {
		it.next(&data3);
		if (cnt == 0)
			EXPECT_TRUE(data3.get() == data1.get());
		else
			EXPECT_TRUE(data3.get() == data2.get());
		cnt++;
	}

	delete normalList;
}


TEST(EtchList, copyTest) {
  EtchList<EtchInt32> o1;
  o1.add(EtchInt32(2));
  o1.add(EtchInt32(3));
  EtchList<EtchInt32> o2(o1);
  EtchList<EtchInt32> o3 = o2;

  EXPECT_EQ(o1.size(), o2.size());
  EXPECT_EQ(o2.size(), o3.size());
}

TEST(EtchList, isInstanceOf) {
  EtchObject* o1 = new EtchList<EtchInt32>();
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchList<EtchInt32>::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchString::TYPE()));
  delete o1;
}


