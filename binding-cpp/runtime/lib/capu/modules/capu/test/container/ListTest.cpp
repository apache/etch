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
#include "capu/container/List.h"
#include "capu/Error.h"
#include "capu/Config.h"

TEST(List, Constructor_Default) {
  //create an empty linked list
  capu::List<capu::int32_t*>* list = NULL;
  list = new capu::List<capu::int32_t*>();
  EXPECT_TRUE(list != NULL);
  delete list;
}

TEST(List, addTest) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::status_t result;

  data1 = 32;
  data2 = 43;
  //append element to the linked list
  result = list->add(data1);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //append another element to linked list
  result = list->add(data2);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //Get added elements to compare that if they are correctly added or not
  list->get(0, &data3);
  EXPECT_TRUE(data3 == data1);

  list->get(1, &data3);
  EXPECT_TRUE(data3 == data2);

  delete list;
}

TEST(List, addIndexTest) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::status_t result;

  data1 = 32;
  data2 = 43;

  result = list->add(10, data1);
  EXPECT_TRUE(result == capu::CAPU_EINVAL);


  result = list->add(0, data2);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->add(1, data2);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->add(0, data2);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->add(1, data1);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //Get added elements to compare that if they are correctly added or not
  list->get(0, &data3);
  EXPECT_TRUE(data3 == data2);

  list->get(1, &data3);
  EXPECT_TRUE(data3 == data1);

  delete list;
}

TEST(List, removeAt) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::status_t result;

  data1 = 32;
  data2 = 43;
  data3 = 56;
  //add some elements to linked list
  result = list->add(data1);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->add(data3);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->add(data2);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //removing element at index 1
  result = list->removeAt(1);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //removing element at index 1
  result = list->removeAt(1);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //removing element at index 0 (HEAD)
  result = list->removeAt(0, &data1);
  EXPECT_TRUE(result == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 32);

  //remove element from out of index
  result = list->removeAt(1000);
  EXPECT_TRUE(result == capu::CAPU_EINVAL);

  //check size of list
  EXPECT_TRUE(list->size() == 0);

  delete list;
}

TEST(List, get) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::int32_t data4;

  capu::status_t result;

  data1 = 32;
  data2 = 43;
  data3 = 56;

  //add some element to the linked list
  result = list->add(data1);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->add(data3);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->add(data2);
  EXPECT_TRUE(result == capu::CAPU_OK);

  //get the added elements by using its index and compare the values with inserted elements
  result = list->get(0, &data4);
  EXPECT_TRUE(result == capu::CAPU_OK);
  EXPECT_TRUE(data1 == data4);

  result = list->get(1, &data4);
  EXPECT_TRUE(result == capu::CAPU_OK);
  EXPECT_TRUE(data3 == data4);

  result = list->get(2, &data4);
  EXPECT_TRUE(result == capu::CAPU_OK);
  EXPECT_TRUE(data2 == data4);

  result = list->get(123, NULL);
  EXPECT_TRUE(result == capu::CAPU_EINVAL);

  delete list;
}

TEST(List, size) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;

  capu::int32_t result;

  data1 = 32;
  data2 = 43;
  data3 = 56;

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

  list->removeAt(0);
  result = list->size();
  EXPECT_TRUE(result == 2);

  list->removeAt(1);
  result = list->size();
  EXPECT_TRUE(result == 1);

  delete list;
}

TEST(List, empty) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;

  capu::bool_t result;

  data1 = 32;
  data2 = 43;
  data3 = 56;

  //check the empty list
  result = list->isEmpty();
  EXPECT_TRUE(result == true);

  //add some element
  list->add(data1);
  result = list->isEmpty();
  EXPECT_TRUE(result == false);

  delete list;
}

TEST(List, find) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::int32_t result;

  data1 = 32;
  data2 = 43;
  data3 = 44;

  //check empty list
  result = list->find(data1);
  EXPECT_TRUE(result == -1);

  //add some elements
  list->add(data1);
  list->add(data2);
  list->add(data3);

  //find the elements
  result = list->find(data1);
  EXPECT_TRUE(result == 0);

  result = list->find(data2);
  EXPECT_TRUE(result == 1);

  result = list->find(data3);
  EXPECT_TRUE(result == 2);

  delete list;
}

TEST(List, contains) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;

  capu::int32_t check_value = 0;
  capu::bool_t result;

  data1 = 32;
  data2 = 43;
  data3 = 44;

  //check empty list
  result = list->contains(check_value);
  EXPECT_TRUE(result == false);

  // fill the linke
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

TEST(List, clear) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::status_t result;

  data1 = 32;
  data2 = 43;
  data3 = 44;

  //add some dummy values to the linked list
  list->add(data1);
  list->add(data1);
  list->add(data2);
  list->add(data3);

  //remove all elements from the linked list
  result = list->clear();
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->add(data1);
  EXPECT_TRUE(result == capu::CAPU_OK);

  result = list->get(0, &data2);
  EXPECT_TRUE(data1 == data2);
  delete list;
}

TEST(List, set) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;

  data1 = 32;
  data2 = 43;

  EXPECT_TRUE(list->add(data1) == capu::CAPU_OK);
  EXPECT_TRUE(list->add(data2) == capu::CAPU_OK);
  EXPECT_TRUE(list->set(1, data1) == capu::CAPU_OK);
  EXPECT_TRUE(list->set(0, data1) == capu::CAPU_OK);
  EXPECT_TRUE(list->get(1, &data2) == capu::CAPU_OK);
  EXPECT_TRUE(list->set(2, data1) == capu::CAPU_EINVAL);
  EXPECT_TRUE(data2 == data1);
  delete list;
}

TEST(ListIterator, hasNext) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::List<capu::int32_t>::Iterator it = list->begin();
  EXPECT_TRUE(it.hasNext() == false);

  data1 = 32;
  data2 = 43;

  list->add(data1);
  list->add(data2);

  it = list->begin();
  capu::bool_t resultb = it.hasNext();
  EXPECT_TRUE(resultb == true);
  delete list;
}

TEST(ListIterator, next) {
  capu::List<capu::int32_t>* list = new capu::List<capu::int32_t > ();
  capu::int32_t data1;
  capu::int32_t data2;
  capu::int32_t data3;
  capu::List<capu::int32_t>::Iterator it = list->begin();
  int cnt = 0;
  EXPECT_TRUE(it.hasNext() == false);
  data1 = 32;
  data2 = 43;
  list->add(data1);
  list->add(data2);
  it = list->begin();
  while (it.hasNext()) {
    it.next(&data3);
    if (cnt == 0)
      EXPECT_TRUE(data3 == data1);
    else
      EXPECT_TRUE(data3 == data2);
    cnt++;
  }
  delete list;
}

TEST(ListIterator, current){
  capu::List<capu::int32_t> list;
  capu::int32_t data1 = 0;
  capu::List<capu::int32_t>::Iterator it = list.begin();
  
  EXPECT_TRUE(it.current(&data1) == capu::CAPU_ERANGE);
  list.add(1);
  it = list.begin();
  EXPECT_TRUE(it.current(&data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 1);
  list.add(2);
  list.add(3);


  data1 = 0;
  capu::int32_t index = 1;
  for(it = list.begin(); it.hasNext(); it.next())
  {
    EXPECT_TRUE(it.current(&data1) == capu::CAPU_OK);
    EXPECT_TRUE(data1 == index);
    ++index;
  }
}

TEST(ListIterator, removeAt){
  capu::List<capu::int32_t> list;
  capu::int32_t data1 = 0;
  capu::List<capu::int32_t>::Iterator it = list.begin();

  EXPECT_TRUE(list.removeAt(it) == capu::CAPU_EINVAL);
  list.add(1);
  it = list.begin();
  EXPECT_TRUE(list.removeAt(it, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(list.size() == 0);
  EXPECT_TRUE(data1 == 1);

  list.add(1);
  list.add(2);
  list.add(3);

  it = list.begin();
  it.next();
  list.removeAt(it, &data1);
  EXPECT_TRUE(data1 == 2);

  capu::int32_t index = 1;

  for(it = list.begin(); it.hasNext();)
  {
    it.next(&data1);
    EXPECT_TRUE(data1 == index);
    index += 2;
  }

  list.add(1,2);

  index = 1;

  it = list.begin();
  it.current(&data1);
  while(it.hasNext())
  {
    list.removeAt(it, &data1);
    EXPECT_TRUE(data1 == index);
    ++index;
  }
}

TEST(ListIterator, add){
    capu::List<capu::int32_t> list;
  capu::List<capu::int32_t>::Iterator iter;
  capu::int32_t data1 = 0;

  list.add(iter, 1);
  list.add(iter, 2);
  list.add(iter, 3);
  EXPECT_TRUE(list.get(0, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 1);
  EXPECT_TRUE(list.get(1, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 2);
  EXPECT_TRUE(list.get(2, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 3);

  iter = list.begin();
  iter.next();
  list.add(iter, 4);
  EXPECT_TRUE(list.get(0, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 1);
  EXPECT_TRUE(list.get(1, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 4);
  EXPECT_TRUE(list.get(2, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 2);
  EXPECT_TRUE(list.get(3, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 3);

  
  iter.next();
  iter.next();
  list.add(iter, 5);
  EXPECT_TRUE(list.get(0, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 1);
  EXPECT_TRUE(list.get(1, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 4);
  EXPECT_TRUE(list.get(2, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 2);
  EXPECT_TRUE(list.get(3, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 3);
  EXPECT_TRUE(list.get(4, &data1) == capu::CAPU_OK);
  EXPECT_TRUE(data1 == 5);

  iter = list.begin();
  list.add(iter, 6);
  EXPECT_TRUE(list.get(0, &data1) == capu::CAPU_OK);
  EXPECT_EQ(6, data1);
  EXPECT_TRUE(list.get(1, &data1) == capu::CAPU_OK);
  EXPECT_EQ(1, data1);
  EXPECT_TRUE(list.get(2, &data1) == capu::CAPU_OK);
  EXPECT_EQ(4, data1);
  EXPECT_TRUE(list.get(3, &data1) == capu::CAPU_OK);
  EXPECT_EQ(2, data1);
  EXPECT_TRUE(list.get(4, &data1) == capu::CAPU_OK);
  EXPECT_EQ(3, data1);
  EXPECT_TRUE(list.get(5, &data1) == capu::CAPU_OK);
  EXPECT_EQ(5, data1);

}

