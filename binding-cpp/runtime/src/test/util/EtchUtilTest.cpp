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
#include "util/EtchUtil.h"

// Tests etch_strcpy_s
TEST(EtchUtilTest, etch_strcpy_s) {
  char* src = NULL;
  char* dst = NULL;
  int dstSize = 0;

  status_t retval = etch_strcpy_s(dst, dstSize, src);
  EXPECT_TRUE(retval == ETCH_EINVAL);

  src = "Test";
  dst = new char[5];
  retval = etch_strcpy_s(dst, dstSize, src);
  EXPECT_TRUE(retval == ETCH_ERANGE);

  dstSize = 2;
  retval = etch_strcpy_s(dst, dstSize, src);
  EXPECT_TRUE(retval == ETCH_EINVAL);

  dstSize = 5;
  retval = etch_strcpy_s(dst, dstSize, src);
  EXPECT_TRUE(retval == ETCH_OK);

  delete[] dst;
}
