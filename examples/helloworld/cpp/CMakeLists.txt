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
project (etch-cpp-helloworld)


IF(UNIX)
  add_definitions("-DOS_LINUX")
  add_definitions("-DARCH_X86_32")
ELSEIF(WIN32)
  add_definitions("-DOS_WINDOWS")
  add_definitions("-DARCH_X86_32")
ENDIF()

#====================================================================================#

#Etch Home
IF (NOT ETCH_HOME)
    MESSAGE(FATAL_ERROR "ETCH_HOME not set")
ENDIF (NOT ETCH_HOME)

SET(ETCH_CPP_HOME ${ETCH_HOME}/binding-cpp)
SET(ETCH_CPP_INCLUDE_DIR ${ETCH_CPP_HOME}/include)
SET(ETCH_CPP_LIBRARY_DIR ${ETCH_CPP_HOME}/lib)

IF(UNIX)
	FIND_LIBRARY(ETCH_LIBRARY NAMES libetch.a PATHS ${ETCH_CPP_LIBRARY_DIR})
	FIND_LIBRARY(CAPU_LIBRARY NAMES libcapu.a PATHS ${ETCH_CPP_LIBRARY_DIR})
ELSEIF(WIN32)
	FIND_LIBRARY(ETCH_LIBRARY NAMES etch.lib PATHS ${ETCH_CPP_LIBRARY_DIR})
	FIND_LIBRARY(CAPU_LIBRARY NAMES capu.lib PATHS ${ETCH_CPP_LIBRARY_DIR}/Windows_X86_32/Debug)
ENDIF()

IF (ETCH_LIBRARY)
    SET(ETCH_LIBRARY_FOUND TRUE)
ENDIF ()

IF (CAPU_LIBRARY)
    SET(CAPU_LIBRARY_FOUND TRUE)
ENDIF ()

IF (NOT ETCH_LIBRARY_FOUND)
    MESSAGE(FATAL_ERROR "ERROR: Could not find etch library in ${ETCH_CPP_LIBRARY_DIR}")
ENDIF ()

IF (NOT CAPU_LIBRARY_FOUND)
    MESSAGE(FATAL_ERROR "ERROR: Could not find capu library in ${ETCH_CPP_LIBRARY_DIR}")
ENDIF ()


# set include dirs
include_directories (${ETCH_CPP_INCLUDE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/src/main/include)
include_directories (${PROJECT_SOURCE_DIR}/target/generated-sources/cpp/include)

#STATUS
message(STATUS " ")
message(STATUS "====================================================")
message(STATUS "using Etch home Dir:     ${ETCH_HOME}")
message(STATUS "using Etch dir:         ${ETCH_LIBRARY}")
message(STATUS "using Capu dir:         ${CAPU_LIBRARY}")

message(STATUS "using include dir: ${ETCH_CPP_INCLUDE_DIR}")
message(STATUS "====================================================")
message(STATUS " ")


#====================================================================================#
# SERVER
#====================================================================================#



add_executable(etch-cpp-helloworld-server
 src/main/src/ImplHelloWorldServer.cpp
 src/main/src/MainHelloWorldListener.cpp
 target/generated-sources/cpp/src/HelloWorld.cpp
 target/generated-sources/cpp/src/HelloWorldClient.cpp

 target/generated-sources/cpp/src/HelloWorldServer.cpp
 target/generated-sources/cpp/src/BaseHelloWorldClient.cpp
 target/generated-sources/cpp/src/BaseHelloWorldServer.cpp
 target/generated-sources/cpp/src/HelloWorldHelper.cpp

 target/generated-sources/cpp/src/RemoteHelloWorld.cpp

 target/generated-sources/cpp/src/RemoteHelloWorldClient.cpp
 target/generated-sources/cpp/src/RemoteHelloWorldServer.cpp
 target/generated-sources/cpp/src/StubHelloWorld.cpp
 target/generated-sources/cpp/src/StubHelloWorldClient.cpp
 target/generated-sources/cpp/src/StubHelloWorldServer.cpp
 target/generated-sources/cpp/src/ValueFactoryHelloWorld.cpp
)


IF (UNIX)
target_link_libraries(etch-cpp-helloworld-server ${ETCH_LIBRARY} ${CAPU_LIBRARY} pthread rt)
set_target_properties(etch-cpp-helloworld-server PROPERTIES COMPILE_FLAGS "-m32 -g" LINK_FLAGS "-m32")
ELSEIF(WIN32)
target_link_libraries(etch-cpp-helloworld-server ${ETCH_LIBRARY} ${CAPU_LIBRARY})
ENDIF ()
set_target_properties(etch-cpp-helloworld-server PROPERTIES OUTPUT_NAME "helloworld-server")

#====================================================================================#
# CLIENT
#====================================================================================#

add_executable(etch-cpp-helloworld-client
 src/main/src/ImplHelloWorldClient.cpp
 src/main/src/MainHelloWorldClient.cpp
 target/generated-sources/cpp/src/HelloWorld.cpp
 target/generated-sources/cpp/src/HelloWorldClient.cpp
 target/generated-sources/cpp/src/HelloWorldServer.cpp
 target/generated-sources/cpp/src/BaseHelloWorldClient.cpp
 target/generated-sources/cpp/src/BaseHelloWorldServer.cpp
 target/generated-sources/cpp/src/HelloWorldHelper.cpp
 target/generated-sources/cpp/src/RemoteHelloWorld.cpp
 target/generated-sources/cpp/src/RemoteHelloWorldClient.cpp
 target/generated-sources/cpp/src/RemoteHelloWorldServer.cpp
 target/generated-sources/cpp/src/StubHelloWorld.cpp
 target/generated-sources/cpp/src/StubHelloWorldClient.cpp
 target/generated-sources/cpp/src/StubHelloWorldServer.cpp
 target/generated-sources/cpp/src/ValueFactoryHelloWorld.cpp
)


IF (UNIX)
target_link_libraries(etch-cpp-helloworld-client ${ETCH_LIBRARY} ${CAPU_LIBRARY} pthread rt)
set_target_properties(etch-cpp-helloworld-client PROPERTIES COMPILE_FLAGS "-m32 -g" LINK_FLAGS "-m32")
ELSEIF(WIN32)
target_link_libraries(etch-cpp-helloworld-client ${ETCH_LIBRARY} ${CAPU_LIBRARY})
ENDIF ()

set_target_properties(etch-cpp-helloworld-client PROPERTIES OUTPUT_NAME "helloworld-client")



