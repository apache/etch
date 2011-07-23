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

IF (WIN32 AND BUILD_CHECK_MEMORY)
  include_directories (${VLD}/include)
  add_definitions (-DBUILD_CHECK_MEMORY)
ENDIF (WIN32 AND BUILD_CHECK_MEMORY)

add_executable (etch-cpp-test
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
    ${GTEST}/src/gtest-all.cc
    main.cpp
)

target_link_libraries (etch-cpp-test etch-cpp)

IF (WIN32 AND BUILD_CHECK_MEMORY)
  file (COPY ${VLD}/vld.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  file (COPY ${VLD}/bin/dbghelp.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  file (COPY ${VLD}/bin/vld.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})

  target_link_libraries (etch-cpp-test ${VLD}/lib/vld.lib)
ENDIF (WIN32 AND BUILD_CHECK_MEMORY)

IF (UNIX)
  set_target_properties (etch-cpp-test PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32 -pthread")
ENDIF (UNIX)
