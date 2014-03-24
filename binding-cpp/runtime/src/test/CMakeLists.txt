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
include_directories (${GMOCK})
include_directories (${GMOCK}/include)
include_directories (${LIBCAPU_INCLUDE_DIR})


IF (TARGET_OS STREQUAL "Windows")
  IF (BUILD_CHECK_MEMORY)
    include_directories (${VLD}/include)
    add_definitions (-DBUILD_CHECK_MEMORY)
  ENDIF()
ENDIF ()

link_directories(${LIBCAPU_LIBRARY_DIR})

add_executable (etch-cpp-test
    common/EtchNumberTest.cpp
	common/EtchInt32Test.cpp
    common/EtchBoolTest.cpp
    common/EtchByteTest.cpp
    common/EtchShortTest.cpp
    common/EtchLongTest.cpp
    common/EtchFloatTest.cpp
    common/EtchDoubleTest.cpp
    common/EtchStringTest.cpp
    common/EtchListTest.cpp
    common/EtchNativeArrayTest.cpp
    common/EtchHashTableTest.cpp
    common/EtchHashSetTest.cpp
    common/EtchExceptionTest.cpp
    common/EtchAuthenticationExceptionTest.cpp
    common/EtchRuntimeExceptionTest.cpp
    common/EtchDateTest.cpp
    common/EtchArrayValueTest.cpp
    util/EtchURLTest.cpp
    util/EtchResourcesTest.cpp
    util/EtchUtilTest.cpp
    util/EtchCircularQueueTest.cpp
    transport/EtchFlexBufferTest.cpp
    transport/EtchTcpConnectionTest.cpp
    transport/EtchTcpListenerTest.cpp
    transport/EtchPacketizerTest.cpp
    transport/EtchMessageTest.cpp
    transport/EtchMessagizerTest.cpp
    transport/EtchPlainMailboxManagerTest.cpp
    transport/EtchDefaultDeliveryServiceTest.cpp
    transport/EtchWhoTest.cpp
    serialization/EtchValidatorBooleanTest.cpp
    serialization/EtchValidatorByteTest.cpp
    serialization/EtchValidatorIntTest.cpp
    serialization/EtchValidatorShortTest.cpp
    serialization/EtchValidatorFloatTest.cpp
    serialization/EtchValidatorDoubleTest.cpp
    serialization/EtchValidatorLongTest.cpp
    serialization/EtchValidatorStringTest.cpp
    serialization/EtchValidatorObjectTest.cpp
    serialization/EtchComboValidatorTest.cpp
    serialization/EtchFieldTest.cpp
    serialization/EtchFieldMapTest.cpp
    serialization/EtchTypeTest.cpp
    serialization/EtchStructValueTest.cpp
    serialization/EtchTypeMapTest.cpp
    serialization/EtchClass2TypeMapTest.cpp
    serialization/EtchListSerializerTest.cpp
    serialization/EtchValidatorRuntimeExceptionTest.cpp
    serialization/EtchRuntimeExceptionSerializerTest.cpp
    serialization/EtchAuthenticationExceptionSerializerTest.cpp
    serialization/EtchHashTableSerializerTest.cpp
    serialization/EtchSetSerializerTest.cpp
    serialization/EtchDateSerializerTest.cpp
    serialization/EtchDefaultValueFactoryTest.cpp
    serialization/EtchValidatorStructValueTest.cpp
    serialization/EtchBinaryTaggedDataInputOutputTest.cpp
    serialization/EtchValidatorCustomTest.cpp
    support/EtchFreePoolTest.cpp
    support/EtchQueuedPoolTest.cpp
    support/EtchPlainMailboxTest.cpp
    support/EtchMonitorTest.cpp
    support/EtchRemoteBaseTest.cpp
    support/EtchRuntimeTest.cpp
    support/EtchAsyncResultTest.cpp
    ${GTEST}/src/gtest-all.cc
    ${GMOCK}/src/gmock-all.cc
    main.cpp
)

IF (TARGET_OS STREQUAL "Linux")
  target_link_libraries (etch-cpp-test etch-cpp capu pthread rt)
ELSEIF(TARGET_OS STREQUAL "Windows")
  target_link_libraries (etch-cpp-test etch-cpp capu)
ELSEIF(TARGET_OS STREQUAL "QNX")
  target_link_libraries (etch-cpp-test etch-cpp capu c socket)
ELSEIF(TARGET_OS STREQUAL "MacOSX")
  target_link_libraries (etch-cpp-test etch-cpp capu)
ELSEIF(TARGET_OS STREQUAL "iPhoneSimulatorOS")
  target_link_libraries (etch-cpp-test etch-cpp capu)
ENDIF()

IF (TARGET_OS STREQUAL "Windows" AND BUILD_CHECK_MEMORY)
  file (COPY ${VLD}/vld.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  file (COPY ${VLD}/bin/dbghelp.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  file (COPY ${VLD}/bin/vld.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})

  target_link_libraries (etch-cpp-test ${VLD}/lib/vld.lib)
ENDIF ()


add_dependencies (etch-cpp-test etch-cpp Capu)

#install test executables
INSTALL(TARGETS etch-cpp-test DESTINATION "${ETCH_INSTALL_DIR}/bin/${ETCH_TARGET_PLATFORM}/Release" CONFIGURATIONS Release)
INSTALL(TARGETS etch-cpp-test DESTINATION "${ETCH_INSTALL_DIR}/bin/${ETCH_TARGET_PLATFORM}/Debug" CONFIGURATIONS Debug)

