/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "gtest/gtest.h"
#include "common/EtchArrayValue.h"
#include "common/EtchInt32.h"
#include "common/EtchDouble.h"
#include "common/EtchFloat.h"
#include "common/EtchBool.h"
#include "capu/util/SmartPointer.h"

TEST(EtchArrayValueTest, ConstructorTest1) {
  EtchNativeArray<capu::SmartPointer<EtchInt32> > *array = new EtchNativeArray<capu::SmartPointer<EtchInt32> >(0);
  EtchArrayValue *av = new EtchArrayValue(array, 0);
  EXPECT_TRUE(av->getTypeCode() == 0);
  EXPECT_TRUE(av->getCustomStructType() == NULL);
  EXPECT_TRUE(av->getDim() == 1);
  delete av;
  delete array;
}

TEST(EtchArrayValueTest, ConstructorTest2) {
  EtchNativeArray<capu::SmartPointer<EtchInt32> > *array = new EtchNativeArray<capu::SmartPointer<EtchInt32> >(0);
  EtchString type("t1");
  EtchType *t1 = new EtchType(type);
  EtchArrayValue *av = new EtchArrayValue(array, 0, 1, t1, 2);
  EXPECT_EQ(1, av->getTypeCode());
  EXPECT_TRUE(t1->equals(av->getCustomStructType()));
  EXPECT_EQ(2, av->getDim());
  delete av;
  delete t1;
  delete array;
}

TEST(EtchArrayValueTest, ConstructorTest3) {
  EtchNativeArray<capu::SmartPointer<EtchInt32> > *array = new EtchNativeArray<capu::SmartPointer<EtchInt32> >(0);
  EtchString type("t1");
  EtchType *t1 = new EtchType(type);
  EtchArrayValue *av = new EtchArrayValue(array, 0, 3, t1, 4);
  EXPECT_EQ(3, av->getTypeCode());
  EXPECT_TRUE(t1->equals(av->getCustomStructType()));
  EXPECT_EQ(4, av->getDim());
  delete av;
  delete t1;
  delete array;
}

TEST(EtchArrayValueTest, getTest) {
  EtchNativeArray<capu::SmartPointer<EtchObject> > *array = new EtchNativeArray<capu::SmartPointer<EtchObject> >(5);

  capu::SmartPointer<EtchObject> array0 = new EtchInt32(5);
  capu::SmartPointer<EtchObject> array1 = new EtchDouble(5);
  capu::SmartPointer<EtchObject> array2 = new EtchFloat(5);
  capu::SmartPointer<EtchObject> array3 = new EtchBool(true);
  capu::SmartPointer<EtchObject> array4 = new EtchString("abc");
  array->set(0, array0);
  array->set(1, array1);
  array->set(2, array2);
  array->set(3, array3);
  array->set(4, array4);

  capu::SmartPointer<EtchObject> result;
  EtchArrayValue *av = new EtchArrayValue(array, 5);

  EXPECT_EQ(5, av->getSize());
  capu::int32_t index = 0;

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array0.get()));

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array1.get()));

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array2.get()));

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array3.get()));

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array4.get()));

  array = (EtchNativeArray<capu::SmartPointer<EtchObject> > *) av->getArray();
  delete array;
  delete av;
}

TEST(EtchArrayValueTest, addTest) {
  capu::SmartPointer<EtchObject> dt = new EtchBool(false);
  EtchNativeArray<capu::SmartPointer<EtchObject> > *array = new EtchNativeArray<capu::SmartPointer<EtchObject> >(5);

  capu::SmartPointer<EtchObject> array0 = new EtchInt32(5);
  capu::SmartPointer<EtchObject> array1 = new EtchDouble(5);
  capu::SmartPointer<EtchObject> array2 = new EtchFloat(5);
  capu::SmartPointer<EtchObject> array3 = new EtchBool(true);
  capu::SmartPointer<EtchObject> array4 = new EtchString("abc");
  array->set(0, array0);
  array->set(1, array1);
  array->set(2, array2);
  array->set(3, array3);
  array->set(4, array4);
  capu::SmartPointer<EtchObject> result;
  EtchArrayValue *av = new EtchArrayValue(array, 5);

  EXPECT_EQ(5, av->getSize());
  capu::int32_t index = 0;

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array0.get()));

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array1.get()));

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array2.get()));

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array3.get()));

  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(array4.get()));

  //correct add
  av->add(dt);
  EXPECT_EQ(ETCH_OK, av->get(index++, result));
  EXPECT_TRUE(result->equals(dt.get()));
  //size should be doubled
  EXPECT_EQ(10, av->getSize());

  av->add(NULL);
  EXPECT_EQ(10, av->getSize());
  array = (EtchNativeArray<capu::SmartPointer<EtchObject> > *) av->getArray();

  delete array;
  delete av;
}



