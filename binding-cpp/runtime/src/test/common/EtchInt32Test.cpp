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

// Tests positive input.
TEST(EtchInt32Test, Constructor_Default) {
    EtchInt32* i1 = new EtchInt32();
    EXPECT_TRUE(i1->getObjectTypeId() == EtchInt32::TYPE_ID);
    EXPECT_TRUE(i1->get() == 0);
    delete i1;
}

TEST(EtchInt32Test, Constructor_Int) {
    EtchInt32* i1 = new EtchInt32(42);
    EXPECT_TRUE(i1->getObjectTypeId() == EtchInt32::TYPE_ID);
    EXPECT_TRUE(i1->get() == 42);
    delete i1;
}

TEST(EtchInt32Test, set) {
    EtchInt32* i1 = new EtchInt32();
    i1->set(42);
    EXPECT_TRUE(i1->get() == 42);
    delete i1;
}

TEST(EtchInt32Test, get) {
    EtchInt32* i1 = new EtchInt32();
    EXPECT_TRUE(i1->get() == 0);
    i1->set(42);
    EXPECT_TRUE(i1->get() == 42);
    delete i1;
}
