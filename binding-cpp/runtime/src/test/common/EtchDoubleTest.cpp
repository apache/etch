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
#include "common/EtchDouble.h"

// Tests positive input.
TEST(EtchDoubleTest, Constructor_Default) {
    EtchDouble* i1 = new EtchDouble();
    EXPECT_TRUE(i1->getObjectTypeId() == EtchDouble::TYPE_ID);
    EXPECT_TRUE(i1->get() == 0.0L);
    delete i1;
}

TEST(EtchDoubleTest, Constructor_Double) {
    EtchDouble* i1 = new EtchDouble(42.0L);
    EXPECT_TRUE(i1->getObjectTypeId() == EtchDouble::TYPE_ID);
    EXPECT_TRUE(i1->get() == 42.0L);
    delete i1;
}

TEST(EtchDoubleTest, set) {
    EtchDouble* i1 = new EtchDouble();
    i1->set(43.0L);
    EXPECT_EQ(i1->get(), 43.0L);
    delete i1;
}

TEST(EtchDoubleTest, get) {
    EtchDouble* i1 = new EtchDouble();
    EXPECT_TRUE(i1->get() == 0.0L);
    i1->set(41.0L);
    EXPECT_TRUE(i1->get() == 41.0L);
    delete i1;
}
