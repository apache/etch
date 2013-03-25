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

# Try to find or download capu
#Once done this will define
#LIBCAPU_FOUND - System has capu
#LIBCAPU_INCLUDE_DIR - The capu include directory
#LIBCAPU_LIBRARY_DIR - The library needed to use capu

IF (NOT DEFINED CMAKE_BUILD_TYPE)
    MESSAGE(FATAL_ERROR "CMAKE_BUILD_TYPE not set")
ENDIF()

IF (NOT DEFINED CAPU_INCLUDE_DIR OR NOT DEFINED CAPU_LIBRARY_DIR)
    include(ExternalProject)

    IF ("${LOCAL_CAPU_SOURCE_DIR}" STREQUAL "")
        #download capu from foreign repository
        SET(CAPU_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd/capu")
        SET(CAPU_CMAKE_BUILD_DIR "${CMAKE_BINARY_DIR}/capu")
        
        ExternalProject_Add(
            Capu
            GIT_REPOSITORY git://github.com/bmwcarit/capu.git
            SOURCE_DIR "${CAPU_PROJECT_DIR}"
            BINARY_DIR "${CAPU_CMAKE_BUILD_DIR}"
            INSTALL_DIR "${CAPU_PROJECT_DIR}/deliverable"
            CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
                       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        )
    ELSE()
        SET(CAPU_PROJECT_DIR "${LOCAL_CAPU_SOURCE_DIR}")
        SET(CAPU_CMAKE_BUILD_DIR "${CAPU_PROJECT_DIR}/build_${TARGET_OS}_${TARGET_ARCH}")

        ExternalProject_Add(
            Capu
            SOURCE_DIR "${CAPU_PROJECT_DIR}"
            BINARY_DIR "${CAPU_CMAKE_BUILD_DIR}"
            DOWNLOAD_COMMAND ""
            UPDATE_COMMAND ""
            INSTALL_COMMAND "install"
            CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
                       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        )
    ENDIF()
    
    SET(CAPU_DELIVERABLE_DIR ${CAPU_PROJECT_DIR}/deliverable/Capu)

    SET(LIBCAPU_INCLUDE_DIR ${CAPU_DELIVERABLE_DIR}/include/Capu)
    SET(LIBCAPU_LIBRARY_DIR ${CAPU_DELIVERABLE_DIR}/lib/${TARGET_OS}_${TARGET_ARCH}/${CMAKE_BUILD_TYPE})
ELSE()
    SET(LIBCAPU_INCLUDE_DIR ${CAPU_INCLUDE_DIR})
    SET(LIBCAPU_LIBRARY_DIR ${CAPU_LIBRARY_DIR})
ENDIF()



include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibCapu  DEFAULT_MSG
                                  LIBCAPU_LIBRARY_DIR LIBCAPU_INCLUDE_DIR)

mark_as_advanced(LIBCAPU_INCLUDE_DIR LIBCAPU_LIBRARY_DIR )

