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
0 *
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <gtest/gtest.h>
#include "capu/util/SmartPointer.h"

class DummyClass {
private:
  capu::int32_t mValue;
  FRIEND_TEST(SmartPointer,Constructors);
  FRIEND_TEST(SmartPointer,Deconstructor);
  FRIEND_TEST(SmartPointer,FileOperator);
public:
  static capu::int32_t mRefCount;

public:
  DummyClass() { 
    mValue = 5;
    mRefCount++;
  }
  ~DummyClass(){
    mRefCount--;
  };

  void set(capu::int32_t value) {
    mValue = value;
  };
};

capu::int32_t DummyClass::mRefCount = 0;



TEST(SmartPointer,Constructors) {	
  {
    capu::SmartPointer<DummyClass> ptr(new DummyClass());
    EXPECT_EQ((capu::uint32_t)1,ptr.getRefCount());
    EXPECT_EQ(1,DummyClass::mRefCount);

    //Copy constructor
    capu::SmartPointer<DummyClass> ptr2(ptr);
    EXPECT_EQ((capu::uint32_t)2,ptr.getRefCount());
    EXPECT_EQ((capu::uint32_t)2,ptr2.getRefCount());
    EXPECT_EQ(1,DummyClass::mRefCount);
    EXPECT_EQ(5,ptr2->mValue);

  }
  EXPECT_EQ(0,DummyClass::mRefCount);
}

TEST(SmartPointer,Deconstructor) {
}    



TEST(SmartPointer,AssignmentOperator) {
  capu::SmartPointer<DummyClass> ptr = new DummyClass();
  EXPECT_EQ((capu::uint32_t)1,ptr.getRefCount());

  //copy
  capu::SmartPointer<DummyClass> ptr2 = ptr;
  EXPECT_EQ((capu::uint32_t)2,ptr.getRefCount());

  capu::SmartPointer<DummyClass> ptr3 = new DummyClass();
  ptr3 = NULL;
  EXPECT_EQ((capu::uint32_t)1,ptr3.getRefCount());

  capu::SmartPointer<DummyClass> ptr4 = ptr3;
  EXPECT_EQ((capu::uint32_t)2,ptr4.getRefCount());
  ptr3 = new DummyClass();
  EXPECT_EQ((capu::uint32_t)1,ptr3.getRefCount());
  EXPECT_EQ((capu::uint32_t)1,ptr4.getRefCount());
}

TEST(SmartPointer,FileOperator) {
  capu::SmartPointer<DummyClass> ptr = new DummyClass();
  EXPECT_EQ(5,ptr->mValue);
  ptr->set(13);
  EXPECT_EQ(13,ptr->mValue);
}

TEST(SmartPointer,BoolOperator) {
  capu::SmartPointer<DummyClass> ptr;
  ptr = new DummyClass();
  ASSERT_TRUE(ptr);
  ptr = NULL;
  ASSERT_FALSE(ptr);
}

TEST(SmartPointer,getRefCount) {
  capu::SmartPointer<DummyClass> ptr;
  ptr = new DummyClass();
  EXPECT_EQ((capu::uint32_t)1, ptr.getRefCount());
  ptr.~SmartPointer();
  EXPECT_EQ((capu::uint32_t)0, ptr.getRefCount());
}
