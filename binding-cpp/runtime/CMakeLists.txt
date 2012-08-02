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

#VLD
SET(VLD ${ETCH_EXTERNAL_DEPENDS}/vld/1.9h)

# GTest
SET(GTEST ${ETCH_EXTERNAL_DEPENDS}/gtest/1.6.0)

# GMock
SET(GMOCK ${ETCH_EXTERNAL_DEPENDS}/gmock/1.6.0)

#Build external CAPU project (OS Abstraction)
SET(CAPU_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/capu")
SET(CAPU_CMAKE_BUILD_DIR "${CAPU_PROJECT_DIR}/build")
SET(CAPU_CMAKE_TOOLCHAIN_DIR "${CAPU_PROJECT_DIR}/cmake/acme/toolchain")
SET(CAPU_DELIVERABLE_DIR "${CAPU_PROJECT_DIR}/deliverable")
IF(UNIX)
  SET(CAPU_CMAKE_TOOLCHAIN_FILE "Linux_X86_32.toolchain")
  add_definitions("-DOS_LINUX")
  add_definitions("-DARCH_X86_32")
ELSEIF(WIN32)
  SET(CAPU_CMAKE_TOOLCHAIN_FILE "Windows_X86_32.toolchain")
  add_definitions("-DOS_WINDOWS")
  add_definitions("-DARCH_X86_32")
ENDIF()

include(ExternalProject)
ExternalProject_Add(
  Capu
  SOURCE_DIR "${CAPU_PROJECT_DIR}"
  BINARY_DIR "${CAPU_CMAKE_BUILD_DIR}"
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE:PATH=${CAPU_CMAKE_TOOLCHAIN_DIR}/${CAPU_CMAKE_TOOLCHAIN_FILE}
             -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
)

#CAPU path
SET(CAPU "${CAPU_PROJECT_DIR}/deliverable/capu")


# Set definitions
IF (UNIX)
  add_definitions (-D_GNU_SOURCE -D_REENTRANT -DLINUX=2 -D__LINUX__)
ENDIF (UNIX)
add_definitions (-D_UNICODE -DUNICODE)

# etch libary
add_subdirectory (src/main)
# etch libary tests
add_subdirectory (src/test)
