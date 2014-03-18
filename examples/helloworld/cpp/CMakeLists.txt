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

IF (EXISTS "${CMAKE_SOURCE_DIR}/CMakeLists_local.txt")
  INCLUDE ("${CMAKE_SOURCE_DIR}/CMakeLists_local.txt" OPTIONAL)
ENDIF()

include (${CMAKE_TOOLCHAIN_FILE})

#====================================================================================#

#Etch Home
IF (NOT ETCH_HOME)
    MESSAGE(FATAL_ERROR "ETCH_HOME not set")
ENDIF (NOT ETCH_HOME)

SET(ETCH_CPP_HOME ${ETCH_HOME}/binding-cpp)
SET(ETCH_CPP_INCLUDE_DIR ${ETCH_CPP_HOME}/include)
SET(ETCH_CPP_LIBRARY_DIR ${ETCH_CPP_HOME}/lib)
SET(ETCH_CPP_LIBRARY_DIR_PLATFORM_SPECIFIC ${ETCH_CPP_LIBRARY_DIR}/${TARGET_OS}_${TARGET_ARCH}/${CMAKE_BUILD_TYPE})

# set include dirs
include_directories (${ETCH_CPP_INCLUDE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/src/main/include)
include_directories (${PROJECT_SOURCE_DIR}/target/generated-sources/cpp/include)

link_directories(${ETCH_CPP_LIBRARY_DIR_PLATFORM_SPECIFIC})


#STATUS
message(STATUS " ")
message(STATUS "====================================================")
message(STATUS "build target:            ${CMAKE_BUILD_TYPE}")
message(STATUS "using Etch home Dir:     ${ETCH_HOME}")
message(STATUS "using include dir:       ${ETCH_CPP_INCLUDE_DIR}")
message(STATUS "using library dir:       ${ETCH_CPP_LIBRARY_DIR_PLATFORM_SPECIFIC}")

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


IF (TARGET_OS STREQUAL "Linux")
  target_link_libraries(etch-cpp-helloworld-server etch capu pthread rt)
ELSEIF(TARGET_OS STREQUAL "Windows")
  target_link_libraries(etch-cpp-helloworld-server etch capu)
  IF (BUILD_CHECK_MEMORY)
    add_definitions(-DBUILD_CHECK_MEMORY)
    include_directories (${VLD}/include)
    file (COPY ${VLD}/vld.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    file (COPY ${VLD}/bin/Win32/dbghelp.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
	file (COPY ${VLD}/bin/Win32/Microsoft.DTfW.DHL.manifest DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    file (COPY ${VLD}/bin/Win32/vld_x86.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    target_link_libraries (etch-cpp-helloworld-server ${VLD}/lib/Win32/vld.lib)
  ENDIF (BUILD_CHECK_MEMORY)
ELSEIF(TARGET_OS STREQUAL "QNX")
  target_link_libraries (etch-cpp-helloworld-server etch capu c socket)
ENDIF()

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

IF (TARGET_OS STREQUAL "Linux")
  target_link_libraries(etch-cpp-helloworld-client etch capu pthread rt)
ELSEIF(TARGET_OS STREQUAL "Windows")
  target_link_libraries(etch-cpp-helloworld-client etch capu )
  IF (BUILD_CHECK_MEMORY)
    add_definitions(-DBUILD_CHECK_MEMORY)
    include_directories (${VLD}/include)
    file (COPY ${VLD}/vld.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    file (COPY ${VLD}/bin/Win32/dbghelp.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
	file (COPY ${VLD}/bin/Win32/Microsoft.DTfW.DHL.manifest DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    file (COPY ${VLD}/bin/Win32/vld_x86.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    target_link_libraries (etch-cpp-helloworld-client ${VLD}/lib/Win32/vld.lib)
  ENDIF (BUILD_CHECK_MEMORY)
ELSEIF(TARGET_OS STREQUAL "QNX")
  target_link_libraries (etch-cpp-helloworld-client etch capu c socket)
ENDIF()

set_target_properties(etch-cpp-helloworld-client PROPERTIES OUTPUT_NAME "helloworld-client")



