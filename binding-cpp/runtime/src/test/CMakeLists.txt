#
# Licensed to the Apache Software Foundation (ASF) under one or more 
# contributor license agreements. See the NOTICE file distributed with  
# this work for additional information regarding copyright ownership. 
# The ASF licenses this file to you under the Apache License, Version  
# 2.0 (the "License"); you may not use this file except in compliance  
# with the License. You may obtain a copy of the License at 
# 
# http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and 
# limitations under the License. 
#

# set include dirs
include_directories (${PROJECT_SOURCE_DIR}/include)
include_directories (${GTEST})
include_directories (${GTEST}/include)

IF (WIN32)
  include_directories (${VLD}/include)
ENDIF (WIN32)

add_executable (etch-cpp-test
    ${GTEST}/src/gtest-all.cc
    ${GTEST}/src/gtest_main.cc
    common/EtchInt32Test.cpp
    common/EtchBoolTest.cpp
    common/EtchByteTest.cpp
    common/EtchShortTest.cpp
    common/EtchLongTest.cpp
    common/EtchFloatTest.cpp
    common/EtchDoubleTest.cpp
    common/EtchStringTest.cpp
    common/EtchObjectTest.cpp
    common/EtchListTest.cpp
    util/EtchUtilTest.cpp
)

target_link_libraries (etch-cpp-test etch-cpp)

IF (WIN32)
  target_link_libraries (etch-cpp-test ${VLD}/lib/vld.lib)
ENDIF (WIN32)

IF (UNIX)
  set_target_properties (etch-cpp-test PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32 -pthread")
ENDIF (UNIX)
