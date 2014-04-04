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

IF ("${CAPU_INCLUDE_DIR}" STREQUAL "" AND "${CAPU_LIBRARY_DIR}" STREQUAL "")
    include(ExternalProject)

    MESSAGE(STATUS "using ${CAPU_INCLUDE_DIR} as CAPU include directory")
    MESSAGE(STATUS "using ${CAPU_LIBRARY_DIR} as CAPU library directory")

    IF ("${LOCAL_CAPU_SOURCE_DIR}" STREQUAL "")
        #download capu from foreign repository
        SET(CAPU_PROJECT_DIR "${CMAKE_BINARY_DIR}/3rd/capu")
        SET(CAPU_CMAKE_BUILD_DIR "${CAPU_BUILD_DIR}/${TARGET_OS}_${TARGET_ARCH}")

        ExternalProject_Add(
            Capu
            URL https://github.com/bmwcarit/capu/archive/v0.14.0.zip
            SOURCE_DIR "${CAPU_PROJECT_DIR}"
            BINARY_DIR "${CAPU_CMAKE_BUILD_DIR}"
            INSTALL_DIR "${CAPU_BUILD_DIR}/deliverable"
            UPDATE_COMMAND ""
            CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
                       -DCMAKE_INSTALL_PREFIX:STRING=${CAPU_BUILD_DIR}/deliverable/${TARGET_OS}_${TARGET_ARCH}/${CMAKE_BUILD_TYPE}
                       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                       -DCONFIG_BUILD_UNITTESTS:BOOLEAN=${CONFIG_BUILD_UNITTESTS}
                       INSTALL 1
        )
    ELSE()
        SET(CAPU_PROJECT_DIR "${LOCAL_CAPU_SOURCE_DIR}")
        SET(CAPU_CMAKE_BUILD_DIR "${CAPU_BUILD_DIR}/${TARGET_OS}_${TARGET_ARCH}")

        ExternalProject_Add(
            Capu
            SOURCE_DIR "${CAPU_PROJECT_DIR}"
            BINARY_DIR "${CAPU_CMAKE_BUILD_DIR}"
            DOWNLOAD_COMMAND ""
            UPDATE_COMMAND ""
            INSTALL_DIR "${CAPU_BUILD_DIR}/deliverable"
            CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
                       -DCMAKE_INSTALL_PREFIX:STRING=${CAPU_BUILD_DIR}/deliverable/${TARGET_OS}_${TARGET_ARCH}/${CMAKE_BUILD_TYPE}
                       -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                       -DCONFIG_BUILD_UNITTESTS:BOOL=${CONFIG_BUILD_UNITTESTS}
                       INSTALL 1
        )
    ENDIF()

    SET(CAPU_DELIVERABLE_DIR ${CAPU_BUILD_DIR}/deliverable/${TARGET_OS}_${TARGET_ARCH}/${CMAKE_BUILD_TYPE})

    SET(LIBCAPU_INCLUDE_DIR ${CAPU_DELIVERABLE_DIR}/include)
    SET(LIBCAPU_LIBRARY_DIR ${CAPU_DELIVERABLE_DIR}/lib)
    SET(LIBCAPU_BINARY_DIR ${CAPU_DELIVERABLE_DIR}/bin)
ELSE()
    SET(LIBCAPU_INCLUDE_DIR ${CAPU_INCLUDE_DIR})
    SET(LIBCAPU_LIBRARY_DIR ${CAPU_LIBRARY_DIR})
    SET(LIBCAPU_BINARY_DIR ${CAPU_BINARY_DIR})
ENDIF()



include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibCapu  DEFAULT_MSG
                                  LIBCAPU_LIBRARY_DIR LIBCAPU_INCLUDE_DIR)

mark_as_advanced(LIBCAPU_INCLUDE_DIR LIBCAPU_LIBRARY_DIR )

