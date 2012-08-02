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
project (etch-c-helloworld)

#if ETCH_HOME has not been set by Ant it is set to the OS environment variable
IF (NOT ETCH_HOME)
    SET(ETCH_HOME $ENV{ETCH_HOME})
ENDIF (NOT ETCH_HOME)

message(STATUS "using etch home ${ETCH_HOME}")

#Etch Home
IF (NOT ETCH_HOME)
    MESSAGE(FATAL_ERROR "ETCH_HOME not set")
ENDIF (NOT ETCH_HOME)

# Set definitions
IF (UNIX)
    add_definitions(-D_GNU_SOURCE -D_REENTRANT -DLINUX=2 -D__LINUX__)
ENDIF (UNIX)
add_definitions(-D_UNICODE -DUNICODE)

# ETCH library
SET(ETCH ${ETCH_HOME}/binding-c)
FIND_PATH(ETCH_INCLUDE_DIR etch.h ${ETCH}/include)
FIND_LIBRARY(ETCH_LIBRARY NAMES etch.lib libetch.a PATHS ${ETCH}/lib NO_DEFAULT_PATH)
IF (ETCH_INCLUDE_DIR AND ETCH_LIBRARY)
    SET(ETCH_FOUND TRUE)
ENDIF (ETCH_INCLUDE_DIR AND ETCH_LIBRARY)
IF (NOT ETCH_FOUND)
    MESSAGE(FATAL_ERROR "Could not find libetch")
ENDIF (NOT ETCH_FOUND)

# APR library
SET(APR ${ETCH_HOME}/binding-c/extern/apr)
FIND_PATH(APR_INCLUDE_DIR apr.h ${APR}/include ${APR}/include/apr-1)
FIND_LIBRARY(APR_LIBRARY NAMES libapr-1.lib libapr-1.so PATHS ${APR}/lib NO_DEFAULT_PATH)
IF (APR_INCLUDE_DIR AND APR_LIBRARY)
    SET(APR_FOUND TRUE)
ENDIF (APR_INCLUDE_DIR AND APR_LIBRARY)
IF (NOT APR_FOUND)
    MESSAGE(FATAL_ERROR "Could not find libapr")
ENDIF (NOT APR_FOUND)

# APR-ICONV library
SET(APR ${ETCH_HOME}/binding-c/extern/apr)
FIND_PATH(APR-ICONV_INCLUDE_DIR apr_iconv.h ${APR}/include ${APR}/include/apr-1)
FIND_LIBRARY(APR-ICONV_LIBRARY NAMES libapriconv-1.lib libapriconv-1.so PATHS ${APR}/lib NO_DEFAULT_PATH)
IF (APR-ICONV_INCLUDE_DIR AND APR-ICONV_LIBRARY)
    SET(APR-ICONV_FOUND TRUE)
ENDIF (APR-ICONV_INCLUDE_DIR AND APR-ICONV_LIBRARY)
IF (NOT APR-ICONV_FOUND)
    MESSAGE(FATAL_ERROR "Could not find libapriconv")
ENDIF (NOT APR-ICONV_FOUND)

# set include dirs
include_directories (${APR_INCLUDE_DIR})
include_directories (${APR-ICONV_INCLUDE_DIR})
include_directories (${ETCH_INCLUDE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/src/main/include)
include_directories (${PROJECT_SOURCE_DIR}/target/generated-sources/c)

add_executable(etch-c-helloworld-server
 src/main/src/helloworld_server_implx.c
 src/main/src/helloworld_server_impl.c
 src/main/src/helloworld_listener_main.c
 target/generated-sources/c/helloworld_interface.c
 target/generated-sources/c/helloworld_client.c
 target/generated-sources/c/helloworld_server_stub.c
 target/generated-sources/c/helloworld_helper.c
 target/generated-sources/c/helloworld_client_stub.c
 target/generated-sources/c/helloworld_remote_client.c
 target/generated-sources/c/helloworld_remote_server.c
 target/generated-sources/c/helloworld_server.c
 target/generated-sources/c/helloworld_valufact.c
 target/generated-sources/c/helloworld_remote.c
)

add_executable(etch-c-helloworld-client
 src/main/src/helloworld_client_impl.c
 src/main/src/helloworld_client_implx.c
 src/main/src/helloworld_client_main.c
 target/generated-sources/c/helloworld_interface.c
 target/generated-sources/c/helloworld_client.c
 target/generated-sources/c/helloworld_server_stub.c
 target/generated-sources/c/helloworld_helper.c
 target/generated-sources/c/helloworld_client_stub.c
 target/generated-sources/c/helloworld_remote_client.c
 target/generated-sources/c/helloworld_remote_server.c
 target/generated-sources/c/helloworld_server.c
 target/generated-sources/c/helloworld_valufact.c
 target/generated-sources/c/helloworld_remote.c
)

target_link_libraries(etch-c-helloworld-server "${ETCH_LIBRARY}")
target_link_libraries(etch-c-helloworld-server "${APR_LIBRARY}")
target_link_libraries(etch-c-helloworld-server "${APR-ICONV_LIBRARY}")
IF (UNIX)
set_target_properties(etch-c-helloworld-server PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
ENDIF (UNIX)
set_target_properties(etch-c-helloworld-server PROPERTIES OUTPUT_NAME "helloworld-server")

target_link_libraries(etch-c-helloworld-client "${ETCH_LIBRARY}")
target_link_libraries(etch-c-helloworld-client "${APR_LIBRARY}")
target_link_libraries(etch-c-helloworld-client "${APR-ICONV_LIBRARY}")
IF (UNIX)
set_target_properties(etch-c-helloworld-client PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
ENDIF (UNIX)
set_target_properties(etch-c-helloworld-client PROPERTIES OUTPUT_NAME "helloworld-client")
