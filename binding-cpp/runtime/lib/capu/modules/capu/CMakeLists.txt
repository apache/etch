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

ADD_MODULE(capu static)

ADD_FILE(Config)
ADD_FILE(Error)
ADD_FILE(os/Mutex SOURCE_GROUP arch_source_group)
ADD_FILE(os/AtomicOperation SOURCE_GROUP arch_source_group)
ADD_FILE(os/Thread SOURCE_GROUP arch_source_group)
ADD_FILE(os/CondVar SOURCE_GROUP arch_source_group)
ADD_FILE(os/StringUtils SOURCE_GROUP arch_source_group)
ADD_FILE(os/NumericLimits SOURCE_GROUP arch_source_group)
ADD_FILE(os/Time SOURCE_GROUP arch_source_group)
ADD_FILE(os/File SOURCE_GROUP arch_source_group)
ADD_FILE(container/List)
ADD_FILE(container/Comparator)
ADD_FILE(container/Hash)
ADD_FILE(container/Pair)
ADD_FILE(container/Array)
ADD_FILE(container/Queue)
ADD_FILE(container/RingBuffer)
ADD_FILE(container/HashTable)
ADD_FILE(container/HashSet)
ADD_FILE(os/Socket SOURCE_GROUP arch_source_group)
ADD_FILE(os/ServerSocket SOURCE_GROUP arch_source_group)
ADD_FILE(os/UdpSocket SOURCE_GROUP arch_source_group)
ADD_FILE(os/Memory SOURCE_GROUP arch_source_group)
ADD_FILE(os/Math SOURCE_GROUP arch_source_group)
ADD_FILE(util/SmartPointer)
ADD_FILE(util/Logger)
ADD_FILE(util/Appender)
ADD_FILE(util/Runnable)
ADD_FILE(util/Traits)
ADD_FILE(util/ThreadPool)

REQUIRED_PACKAGE(Thread)
REQUIRED_PACKAGE(LibRt)
REQUIRED_PACKAGE(Socket)
REQUIRED_PACKAGE(Net) 
REQUIRED_PACKAGE(IP4)
REQUIRED_PACKAGE(Res)

SOURCE_GROUP(${TARGET_OS}_${TARGET_ARCH} FILES  ${arch_source_group})

IF("${TARGET_OS}" STREQUAL "Integrity")
	ADD_FILE(os/Integrity/global_table)
	ADD_LINKER_FLAG("${CMAKE_SOURCE_DIR}/modules/capu/res/kernel_default.ld")
	ADD_LINKER_FLAG("-kernel")
ENDIF()

SET(USE_BOOST 0 CACHE BOOL "Enable boost performance test")
SET(USE_STL_UNORDEREDMAP 0 CACHE BOOL "Enable strl tr1 unordered_map performance test")

IF(${USE_BOOST})
	ADD_COMPILER_FLAG("USE_BOOST")
	SET(BOOST_ROOT_DIR "" CACHE PATH "Path to boost root")
	INCLUDE_DIRECTORIES("${BOOST_ROOT_DIR}")
ENDIF()

IF(${USE_STL_UNORDEREDMAP})
	ADD_COMPILER_FLAG("USE_STL_UNORDEREDMAP")
ENDIF()

JUST_DOIT()
