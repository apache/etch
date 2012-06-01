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
include_directories (${CAPU}/include/capu)

#Add header files
SET(MAIN_INCLUDES
    ${PROJECT_SOURCE_DIR}/include/common/EtchObject.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchObjectType.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchString.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchInt32.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchBool.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchByte.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchShort.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchLong.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchFloat.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchDouble.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchComparator.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchComparatorNative.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchConfig.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchError.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchHash.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchHashNative.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchHashTable.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchHashSet.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchList.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchNativeArray.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchSocket.h
    ${PROJECT_SOURCE_DIR}/include/common/EtchServerSocket.h
    ${PROJECT_SOURCE_DIR}/include/util/EtchURL.h
    ${PROJECT_SOURCE_DIR}/include/util/EtchResources.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchFlexBuffer.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchSession.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchSessionData.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchSessionListener.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchSessionMessage.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchSessionPacket.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchWho.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchConnection.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchTransport.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchTransportData.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchTransportMessage.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchTransportPacket.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchTcpConnection.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchTcpListener.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchTcpOption.h
    ${PROJECT_SOURCE_DIR}/include/transport/EtchPacketizer.h
    ${PROJECT_SOURCE_DIR}/include/serialization/EtchValidator.h
    ${PROJECT_SOURCE_DIR}/include/serialization/EtchTypeValidator.h
    ${PROJECT_SOURCE_DIR}/include/serialization/EtchTypeCodes.h
    ${PROJECT_SOURCE_DIR}/include/serialization/EtchValidatorBoolean.h
    ${PROJECT_SOURCE_DIR}/include/serialization/EtchValidatorByte.h
    ${PROJECT_SOURCE_DIR}/include/util/EtchUtil.h
    )

#Add cpp files
SET(MAIN_SOURCES
    common/EtchObject.cpp
    common/EtchObjectType.cpp
    common/EtchString.cpp
    common/EtchInt32.cpp
    common/EtchBool.cpp
    common/EtchByte.cpp
    common/EtchShort.cpp
    common/EtchLong.cpp
    common/EtchFloat.cpp
    common/EtchDouble.cpp
    common/EtchSocket.cpp
    common/EtchServerSocket.cpp
    util/EtchURL.cpp
    util/EtchResources.cpp
    transport/EtchFlexBuffer.cpp
    transport/EtchSession.cpp
    transport/EtchTcpConnection.cpp
    transport/EtchTcpListener.cpp
    transport/EtchTcpOption.cpp
    transport/EtchPacketizer.cpp
    serialization/EtchTypeValidator.cpp
    serialization/EtchValidatorBoolean.cpp
    serialization/EtchValidatorByte.cpp
    util/EtchUtil.cpp
    )

#Add source and header files to library so they are visible in the different IDEs (Visual Studio and Eclipse)
add_library (etch-cpp STATIC
    ${MAIN_SOURCES}
    ${MAIN_INCLUDES}
)

IF (UNIX)
  target_link_libraries (etch-cpp ${CAPU/lib/Linux})
ELSEIF(WIN32)
  target_link_libraries (etch-cpp ${CAPU/lib/Win32})
ENDIF()

IF (UNIX)
  set_target_properties (etch-cpp PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
ENDIF (UNIX)

set_target_properties (etch-cpp PROPERTIES OUTPUT_NAME "etch")

add_dependencies (etch-cpp Capu)
