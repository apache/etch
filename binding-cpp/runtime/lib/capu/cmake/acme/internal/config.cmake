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

SET(HEADER_FILE_EXTENSIONS "h;hpp;inc" CACHE STRING "file extension of header files.")
SET(SOURCE_FILE_EXTENSIONS "cpp;c" CACHE STRING "file extension of source files.")

SET(BUILD_UNITTESTS 1 CACHE BOOL "Building Unit-Tests")
IF(NOT DEFINED THIRD_PARTY_DIR)
	SET(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3psw)
ENDIF()

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	SET(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/deliverable/${CMAKE_PROJECT_NAME}"  CACHE PATH "Install prefix" FORCE )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

SET(CMAKE_CMAKE_OUTPUT_DIRECTORY    "${CMAKE_INSTALL_PREFIX}/cmake"              CACHE INTERNAL "Path to store information for external use of the project.")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_INSTALL_PREFIX}/lib/${TARGET_OS}_${TARGET_ARCH}" CACHE INTERNAL "Path to store library files.")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_INSTALL_PREFIX}/lib/${TARGET_OS}_${TARGET_ARCH}" CACHE INTERNAL "Path to store archive files.")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_INSTALL_PREFIX}/bin/${TARGET_OS}_${TARGET_ARCH}" CACHE INTERNAL "Path to store executable files.")
SET(CMAKE_HEADER_OUTPUT_DIRECTORY   "${CMAKE_INSTALL_PREFIX}/include"            CACHE INTERNAL "Path to store public header files.")
SET(CMAKE_RESOURCE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/res"                CACHE INTERNAL "Path to store resource files.")
SET(CMAKE_DOC_OUTPUT_DIRECTORY      "${CMAKE_INSTALL_PREFIX}/doc"                CACHE INTERNAL "Path to store documentation files.")


SET(GLOBAL_EXTERN_INCLUDE_DIRS      ""  CACHE INTERNAL "collect extern include dirs")
SET(BUILD_GLOBAL_TEST_EXECUTABLE    0   CACHE BOOL     "Enable building of one test executable")
SET(GLOBAL_TEST_LIBS                ""  CACHE INTERNAL "collect test libs")
SET(GLOBAL_TEST_SOURCE              ""  CACHE INTERNAL "collect test source")
SET(GLOBAL_TEST_INCLUDE_DIRECTORIES ""  CACHE INTERNAL "collect test include directories")

SET(GLOBAL_TEST_DEBUG_COMPILER_FLAGS      ""  CACHE INTERNAL "collect test compiler flags")
SET(GLOBAL_TEST_DEBUG_LINKER_FLAGS        ""  CACHE INTERNAL "collect test linker flags")
SET(GLOBAL_TEST_DEBUG_DEFINITIONS         ""  CACHE INTERNAL "collect test defintions")
SET(GLOBAL_TEST_RELEASE_COMPILER_FLAGS    ""  CACHE INTERNAL "collect test compiler flags")
SET(GLOBAL_TEST_RELEASE_LINKER_FLAGS      ""  CACHE INTERNAL "collect test linker flags")
SET(GLOBAL_TEST_RELEASE_DEFINITIONS       ""  CACHE INTERNAL "collect test defintions")

SET(GLOBAL_TEST_LINKER_DIRECTORIES  ""  CACHE INTERNAL "collect test linker directories")
#SET(GLOBAL_INCLUDE_DIRECTORIES      ""  CACHE INTERNAL "collect include directories")
SET(GLOBAL_LIB_DIRECTORIES          ""  CACHE INTERNAL "collect lib directories")
SET(GLOBAL_LIBRARIES                ""  CACHE INTERNAL "collect all linkable libraries")

SET(UTILS_MODULES_STATIC  "" CACHE INTERNAL "")
SET(UTILS_MODULES_DYNAMIC "" CACHE INTERNAL "")
SET(UTILS_MODULES_EXE     "" CACHE INTERNAL "")
SET(UTILS_MODULES_TESTS   "" CACHE INTERNAL "")

