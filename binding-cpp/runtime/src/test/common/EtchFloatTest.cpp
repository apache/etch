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
#include "common/EtchFloat.h"

// Tests positive input.
TEST(EtchFloatTest, Constructor_Default) {
    EtchFloat* i1 = new EtchFloat();
    EXPECT_TRUE(i1->getObjectTypeId() == EtchFloat::TYPE_ID);
    EXPECT_TRUE(i1->get() == 0.0f);
    delete i1;
}

TEST(EtchFloatTest, Constructor_Float) {
    EtchFloat* i1 = new EtchFloat(42.0f);
    EXPECT_TRUE(i1->getObjectTypeId() == EtchFloat::TYPE_ID);
    EXPECT_TRUE(i1->get() == 42.0f);
    delete i1;
}

TEST(EtchFloatTest, set) {
    EtchFloat* i1 = new EtchFloat();
    i1->set(43.33f);
    EXPECT_TRUE(i1->get() == 43.33f);
    delete i1;
}

TEST(EtchFloatTest, get) {
    EtchFloat* i1 = new EtchFloat();
    EXPECT_TRUE(i1->get() == 0.0f);
    i1->set(41.0f);
    EXPECT_TRUE(i1->get() == 41.0f);
    delete i1;
}
