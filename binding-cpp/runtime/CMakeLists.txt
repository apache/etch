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
cmake_minimum_required (VERSION 2.6) 
project (etch-cpp)

message (STATUS "using external libraries ${ETCH_EXTERNAL_DEPENDS}")

# Etch external
IF (NOT ETCH_EXTERNAL_DEPENDS)
  MESSAGE (FATAL_ERROR "ETCH_EXTERNAL_DEPENDS not set")
ENDIF (NOT ETCH_EXTERNAL_DEPENDS)

#VLD
SET(VLD ${ETCH_EXTERNAL_DEPENDS}/vld/1.9h)


# GTest
SET(GTEST ${ETCH_EXTERNAL_DEPENDS}/gtest/1.5.0)

# Set definitions
IF (UNIX)
  add_definitions (-D_GNU_SOURCE -D_REENTRANT -DLINUX=2 -D__LINUX__)
ENDIF (UNIX)
add_definitions (-D_UNICODE -DUNICODE)

# etch libary
add_subdirectory (src/main)
# etch libary tests
add_subdirectory (src/test)
