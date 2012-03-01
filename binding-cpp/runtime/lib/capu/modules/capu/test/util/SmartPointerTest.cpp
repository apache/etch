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
  FRIEND_TEST(SmartPointer,DereferencingOperator);
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

class ChildDummyClass : public DummyClass {
public:
  ChildDummyClass() {

  }
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

  //constructor for castable type
  capu::SmartPointer<ChildDummyClass> childPtr(new ChildDummyClass());
  capu::SmartPointer<DummyClass> parentPtr(childPtr);
  EXPECT_EQ((capu::uint32_t)2,childPtr.getRefCount());
  EXPECT_EQ((capu::uint32_t)2,parentPtr.getRefCount());
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

   //assignment of castable type
  capu::SmartPointer<ChildDummyClass> childPtr(new ChildDummyClass());
  capu::SmartPointer<DummyClass> parentPtr(new DummyClass());
  capu::SmartPointer<DummyClass> parentPtrCopy(parentPtr);

  EXPECT_EQ((capu::uint32_t)2,parentPtr.getRefCount());
  EXPECT_EQ((capu::uint32_t)2,parentPtrCopy.getRefCount());
  
  //assign
  parentPtr = childPtr;
  EXPECT_EQ((capu::uint32_t)2,childPtr.getRefCount());
  EXPECT_EQ((capu::uint32_t)2,parentPtr.getRefCount());
  EXPECT_EQ((capu::uint32_t)1,parentPtrCopy.getRefCount());

}

TEST(SmartPointer,FileOperator) {
  capu::SmartPointer<DummyClass> ptr = new DummyClass();
  EXPECT_EQ(5,ptr->mValue);
  ptr->set(13);
  EXPECT_EQ(13,ptr->mValue);
}

TEST(SmartPointer,DereferencingOperator) {
  DummyClass *dc = new DummyClass();
  capu::SmartPointer<DummyClass> ptr(dc);
  DummyClass *dc2 = &(*ptr);
  EXPECT_EQ(dc,dc2);
}

TEST(SmartPointer,getObject) {
  DummyClass *dc = new DummyClass();
  capu::SmartPointer<DummyClass> ptr(dc);
  DummyClass *dc2 = ptr.get();
  EXPECT_TRUE(dc2 != NULL);
  EXPECT_EQ(dc,dc2);
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

TEST(SmartPointer, operatorTest) {
  capu::SmartPointer<DummyClass> ptr, ptr2;
  ptr = new DummyClass();
  EXPECT_EQ((capu::uint32_t)1, ptr.getRefCount());
  EXPECT_FALSE(ptr == ptr2);
  EXPECT_TRUE(ptr != ptr2);
  ptr2 = ptr;
  EXPECT_TRUE(ptr == ptr2);
  EXPECT_FALSE(ptr != ptr2);
}

TEST(SmartPointer,castTest) {
  capu::SmartPointer<DummyClass> ptr2;
  capu::SmartPointer<ChildDummyClass> ptr;
  ptr = new ChildDummyClass();
  EXPECT_EQ((capu::uint32_t)1, ptr.getRefCount());
  capu::SmartPointer<DummyClass> gecastet =  ptr;
  EXPECT_EQ((capu::uint32_t)2, ptr.getRefCount());
}
