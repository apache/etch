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
#include "common/EtchNative.h"
#include "common/EtchNativeArray.h"


TEST(EtchNativeArrayTest, Constructor_Default) {
  EtchNativeArray<EtchInt32*, EtchInt32::TYPE_ID>* na =
    new EtchNativeArray<EtchInt32*, EtchInt32::TYPE_ID>(10);
    delete na;
}

TEST(EtchNativeArrayTest, Constructor_Array) {

    EtchInt32* int1 = new EtchInt32(42);
    EtchInt32* int2  = new EtchInt32(43);

    EtchNativeArray<EtchInt32*, EtchInt32::TYPE_ID>* na1 =
      new EtchNativeArray<EtchInt32*, EtchInt32::TYPE_ID>(2);

    na1->set(0, int1);
    na1->set(1, int2);

    EXPECT_TRUE(na1->getObjectTypeId() == (EtchNativeArray<EtchInt32,EtchInt32::TYPE_ID>::TYPE_ID));

    EtchInt32* value = NULL;
    int i = 0;
    for(i = 0; i < 2; i++) {
      status_t ret = na1->get(i, &value);
      ASSERT_EQ(ETCH_OK, ret);
      ASSERT_EQ(42 + i, value->get());
    }

    status_t ret = na1->get(i, &value);
    ASSERT_EQ(ETCH_ERANGE, ret);

    delete na1;
    delete int1;
    delete int2;
}

TEST(EtchNativeArrayTest, setget) {

    EtchInt32* int1 = new EtchInt32(42);
    EtchInt32* int2 = new EtchInt32(43);
    EtchNativeArray<EtchInt32*, EtchInt32::TYPE_ID>* na1 =
      new EtchNativeArray<EtchInt32*, EtchInt32::TYPE_ID>(2);
    EXPECT_TRUE(na1->getObjectTypeId() == (EtchNativeArray<EtchInt32, EtchInt32::TYPE_ID>::TYPE_ID));
    
    na1->set(0, int1);
    na1->set(1, int2);

    EtchInt32* value = NULL;
    int i = 0;
    for(i = 0; i < 2; i++) {
      status_t ret = na1->get(i, &value);
      ASSERT_EQ(ETCH_OK, ret);
      ASSERT_EQ(42 + i, value->get());
    }

    EtchInt32* newValue = new EtchInt32(44);

    na1->set(1, newValue);

    status_t ret = na1->get(1, &value);
    ASSERT_EQ(ETCH_OK, ret);
    ASSERT_EQ(44, newValue->get());

    //Test setget of block
    EtchNativeArray<EtchInt32*, EtchInt32::TYPE_ID> *native_array = new EtchNativeArray<EtchInt32*, EtchInt32::TYPE_ID> (2);
    EtchInt32* int3 = new EtchInt32[2]();
    int3[0].set(2);
    int3[1].set(4);
    
    ret = native_array->set(0,NULL,0,0,0); 
    EXPECT_TRUE(ret == ETCH_EINVAL);
   
    ret = native_array->get(0,NULL,0,0,0);
    EXPECT_TRUE(ret == ETCH_EINVAL);
    
    ret = native_array->set(0,&int3,2,0,2);
    EXPECT_TRUE(ret == ETCH_OK);
    
    ret = native_array->get(0,&int1,2,0,2);
    EXPECT_TRUE(ret == ETCH_OK);
    
    EXPECT_TRUE(int1[0].get() == 2);
    EXPECT_TRUE(int1[1].get() == 4);


    //Test setget of block of native type
    EtchNativeArray<int, EtchNative::INT32> *native_array2 = new EtchNativeArray<int, EtchNative::INT32> (2);
    int int4[2];
    int int5[2];
    int4[0] = 3;
    int4[1] = 5;
    
    ret = native_array2->set(0,NULL,0,0,0); 
    EXPECT_TRUE(ret == ETCH_EINVAL);
   
    ret = native_array2->get(0,NULL,0,0,0);
    EXPECT_TRUE(ret == ETCH_EINVAL);
    
    ret = native_array2->set(0,(int*) int4,2,0,2);
    EXPECT_TRUE(ret == ETCH_OK);
    
    ret = native_array2->get(0,(int* )int5,2,0,2);
    EXPECT_TRUE(ret == ETCH_OK);
    
    EXPECT_TRUE(int5[0] == 3);
    EXPECT_TRUE(int5[1] == 5);
    
    //index out of bounds
    ret = native_array2->set(5,(int*) int4,2,0,2);
    EXPECT_TRUE(ret == ETCH_ERANGE);

    //buffer size smaller then count
    ret = native_array2->set(0,(int*) int4,1,0,2);
    EXPECT_TRUE(ret == ETCH_ERANGE);
    
    //offset + count out of range
    ret = native_array2->set(0,(int*) int4,2,2,2);
    EXPECT_TRUE(ret == ETCH_ERANGE);

    //index out of bounds
    ret = native_array2->get(5,(int*) int5,2,0,2);
    EXPECT_TRUE(ret == ETCH_ERANGE);

    //buffer size smaller then count
    ret = native_array2->get(0,(int*) int5,1,0,2);
    EXPECT_TRUE(ret == ETCH_ERANGE);
    
    //offset + count out of range
    ret = native_array2->set(0,(int*) int5,2,2,2);
    EXPECT_TRUE(ret == ETCH_ERANGE);
    
    
    delete[] int3;
    delete na1;
    delete int2;
    delete newValue;
    delete native_array;
    delete native_array2;
}
