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
cmake_minimum_required (VERSION 2.8)
project (etch-cpp)

message (STATUS "using external libraries ${ETCH_EXTERNAL_DEPENDS}")

IF (EXISTS "${CMAKE_SOURCE_DIR}/CMakeLists_local.txt")
INCLUDE ("${CMAKE_SOURCE_DIR}/CMakeLists_local.txt" OPTIONAL)
ENDIF()

# Set build settings
IF ("${BUILD_CHECK_MEMORY}" STREQUAL "")
  SET(BUILD_CHECK_MEMORY FALSE)
ENDIF()

# Etch external
IF (NOT ETCH_EXTERNAL_DEPENDS)
  MESSAGE (FATAL_ERROR "ETCH_EXTERNAL_DEPENDS not set")
ENDIF (NOT ETCH_EXTERNAL_DEPENDS)

# Check Toolchain file
IF (NOT CMAKE_TOOLCHAIN_FILE)
  MESSAGE (FATAL_ERROR "CMAKE_TOOLCHAIN_FILE not set")
ENDIF (NOT CMAKE_TOOLCHAIN_FILE)

#VLD
SET(VLD ${ETCH_EXTERNAL_DEPENDS}/vld/1.9h)

# GMock
SET(GMOCK ${ETCH_EXTERNAL_DEPENDS}/gmock/1.6.0)

# GTest
SET(GTEST ${GMOCK}/gtest)

#Build external CAPU project (OS Abstraction)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Capu)

include (${CMAKE_TOOLCHAIN_FILE})

# Set definitions
IF (TARGET_OS STREQUAL "Linux")
  add_definitions (-D_GNU_SOURCE -D_REENTRANT -DLINUX=2 -D__LINUX__)
ENDIF ()
add_definitions (-D_UNICODE -DUNICODE)

#set cmake output variables
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# etch libary
add_subdirectory (src/main)
# etch libary tests

IF(CONFIG_BUILD_TESTS)
  add_subdirectory (src/test)
ENDIF()

#install capu headers and library
INSTALL(DIRECTORY ${LIBCAPU_INCLUDE_DIR}/capu DESTINATION "${ETCH_INSTALL_DIR}/include")

INSTALL(DIRECTORY ${LIBCAPU_LIBRARY_DIR}/ DESTINATION "${ETCH_INSTALL_DIR}/lib/${ETCH_TARGET_PLATFORM}/${CMAKE_BUILD_TYPE}" FILES_MATCHING PATTERN "*capu*" PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ)
IF (NOT ${LIBCAPU_BINARY_DIR} STREQUAL "")
  INSTALL(DIRECTORY ${LIBCAPU_BINARY_DIR} DESTINATION "${ETCH_INSTALL_DIR}/bin/${ETCH_TARGET_PLATFORM}/${CMAKE_BUILD_TYPE}" FILES_MATCHING PATTERN "capuTest*" PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ)
ENDIF()

