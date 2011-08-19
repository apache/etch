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
SET(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3psw)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/deliverable/lib/${TARGET_ARCH}" CACHE STRING "Path to store library files.")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/deliverable/lib/${TARGET_ARCH}" CACHE STRING "Path to store archive files.")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/deliverable/bin/${TARGET_ARCH}" CACHE STRING "Path to store executable files.")
SET(CMAKE_HEADER_OUTPUT_DIRECTORY   "${PROJECT_SOURCE_DIR}/deliverable/include"            CACHE STRING "Path to store public header files.")
SET(CMAKE_RESOURCE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/deliverable/res"                CACHE STRING "Path to store resource files.")
SET(CMAKE_DOC_OUTPUT_DIRECTORY      "${PROJECT_SOURCE_DIR}/deliverable/doc"                CACHE STRING "Path to store documentation files.")

SET(BUILD_GLOBAL_TEST_EXECUTABLE    0   CACHE BOOL     "Enable building of one test executable")
SET(GLOBAL_TEST_LIBS                ""  CACHE INTERNAL "collect test libs")
SET(GLOBAL_TEST_SOURCE              ""  CACHE INTERNAL "collect test source")
SET(GLOBAL_TEST_INCLUDE_DIRECTORIES ""  CACHE INTERNAL "collect test include directories")
SET(GLOBAL_TEST_COMPILER_FLAGS      ""  CACHE INTERNAL "collect test compiler flags")
SET(GLOBAL_TEST_LINKER_FLAGS        ""  CACHE INTERNAL "collect test linker flags")
SET(GLOBAL_TEST_DEFINITIONS         ""  CACHE INTERNAL "collect test deifintions")

SET(UTILS_MODULES_STATIC  "" CACHE INTERNAL "")
SET(UTILS_MODULES_DYNAMIC "" CACHE INTERNAL "")
SET(UTILS_MODULES_EXE     "" CACHE INTERNAL "")
SET(UTILS_MODULES_TESTS   "" CACHE INTERNAL "")
