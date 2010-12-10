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
#include "common/EtchString.h"

// Tests positive input.
TEST(EtchStringTest, Constructor_Default) {
    EtchString* s1 = new EtchString();
    EXPECT_TRUE(s1->getObjectTypeId() == EtchString::TYPE_ID);
    EXPECT_TRUE(s1->c_str() == NULL);
    delete s1;
}

TEST(EtchStringTest, Constructor_Char) {
    EtchString* s1 = new EtchString("test");
    EXPECT_TRUE(s1->getObjectTypeId() == EtchString::TYPE_ID);
    EXPECT_TRUE(s1->c_str() != NULL);
    EXPECT_TRUE(strcmp(s1->c_str(), "test") == 0);
    delete s1;
}

TEST(EtchStringTest, set) {
    EtchString* s1 = new EtchString("test");
    s1->set("hello");
    EXPECT_TRUE(s1->c_str() != NULL);
    EXPECT_TRUE(strcmp(s1->c_str(), "hello") == 0);
    delete s1;
}

TEST(EtchStringTest, c_str) {
    EXPECT_TRUE(true);
}
