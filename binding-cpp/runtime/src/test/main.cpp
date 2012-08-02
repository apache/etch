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

#ifdef BUILD_CHECK_MEMORY
#ifdef WIN32
#include "vld.h"
#endif
#endif

#include <iostream>
#include "gtest/gtest.h"

GTEST_API_ int main(int argc, char **argv) {
  std::cout << "Running etch-cpp tests\n";

  bool wait = false;
  // special wait command
  for(int i = 0; i < argc; i++) {
    if(strcmp(argv[i], "--gtest_pause") == 0) {
      wait = true;
      argv[i] = const_cast<char*>("");
    }
  }

  testing::InitGoogleTest(&argc, argv);
  int testResult = RUN_ALL_TESTS();

  if(wait) {
    printf("press <enter> to exit!");
    getchar();
  }

  return testResult;
}
