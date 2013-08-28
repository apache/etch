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
project (etch-c)

message(STATUS "using external libraries ${ETCH_EXTERNAL_DEPENDS}")

# Etch external
IF (NOT ETCH_EXTERNAL_DEPENDS)
    MESSAGE(FATAL_ERROR "ETCH_EXTERNAL_DEPENDS not set")
ENDIF (NOT ETCH_EXTERNAL_DEPENDS)

# Set definitions
IF (UNIX)
    add_definitions(-D_GNU_SOURCE -D_REENTRANT -DLINUX=2 -D__LINUX__)
ENDIF (UNIX)
add_definitions(-D_UNICODE -DUNICODE)

# APR library
SET(APR ${ETCH_EXTERNAL_DEPENDS}/apr/1.4.8)
FIND_PATH(APR_INCLUDE_DIR apr.h ${APR}/include ${APR}/include/apr-1)
FIND_LIBRARY(APR_LIBRARY NAMES libapr-1.lib libapr-1.so PATHS ${APR}/lib NO_DEFAULT_PATH)
IF (APR_INCLUDE_DIR AND APR_LIBRARY)
    SET(APR_FOUND TRUE)
ENDIF (APR_INCLUDE_DIR AND APR_LIBRARY)
IF (NOT APR_FOUND)
    MESSAGE(FATAL_ERROR "Could not find libapr")
ENDIF (NOT APR_FOUND)

# APR-ICONV library
SET(APR ${ETCH_EXTERNAL_DEPENDS}/apr/1.4.8)
FIND_PATH(APR-ICONV_INCLUDE_DIR apr_iconv.h ${APR}/include ${APR}/include/apr-1)
FIND_LIBRARY(APR-ICONV_LIBRARY NAMES libapriconv-1.lib libapriconv-1.so PATHS ${APR}/lib NO_DEFAULT_PATH)
IF (APR-ICONV_INCLUDE_DIR AND APR-ICONV_LIBRARY)
    SET(APR-ICONV_FOUND TRUE)
ENDIF (APR-ICONV_INCLUDE_DIR AND APR-ICONV_LIBRARY)
IF (NOT APR-ICONV_FOUND)
    MESSAGE(FATAL_ERROR "Could not find libapriconv")
ENDIF (NOT APR-ICONV_FOUND)

# CUNIT library
SET(CUNIT ${ETCH_EXTERNAL_DEPENDS}/cunit/2.1)
FIND_PATH(CUNIT_INCLUDE_DIR CUnit.h ${CUNIT}/include /usr/include/CUnit)
FIND_LIBRARY(CUNIT_LIBRARY NAMES libcunit.lib libcunit.a PATHS ${CUNIT}/lib /usr/lib NO_DEFAULT_PATH)
IF (CUNIT_INCLUDE_DIR AND CUNIT_LIBRARY)
    SET(CUNIT_FOUND TRUE)
ENDIF (CUNIT_INCLUDE_DIR AND CUNIT_LIBRARY)
IF (NOT CUNIT_FOUND)
    MESSAGE(FATAL_ERROR "Could not find libcunit")
ENDIF (NOT CUNIT_FOUND)

# etch libary
add_subdirectory (src/main)
# etch libary tests
add_subdirectory (src/test)
