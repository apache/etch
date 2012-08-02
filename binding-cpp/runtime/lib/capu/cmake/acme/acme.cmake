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


#--------------------------------------------------------------------------
# if a toolchain was provided and the toolchain file exists make sure,
# all variables are set as early as possible
#--------------------------------------------------------------------------
IF (NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "" )
	IF (EXISTS "${CMAKE_TOOLCHAIN_FILE}")
		INCLUDE(${CMAKE_TOOLCHAIN_FILE})
		MESSAGE(STATUS "Using toolchain file: " ${CMAKE_TOOLCHAIN_FILE})
	ELSE()
		MESSAGE(STATUS "WARNING: toolchain file not found: " ${CMAKE_TOOLCHAIN_FILE})
	ENDIF()
ENDIF()



#--------------------------------------------------------------------------
# store current path to acme build system
# there the macro CMAKE_CURRENT_LIST_DIR, but it requires Cmake 2.8.3 or higher.
# this solution works on CMake 2.6 as well.
#--------------------------------------------------------------------------
get_filename_component(ACME_PATH ${CMAKE_CURRENT_LIST_FILE} PATH) 
MESSAGE(STATUS "using ACME base directory: ${ACME_PATH}")

#--------------------------------------------------------------------------
# include internal implementation, configuration and plugins
#--------------------------------------------------------------------------
INCLUDE(${ACME_PATH}/internal/config.cmake)
INCLUDE(${ACME_PATH}/internal/macros.cmake)

MACRO(ADD_CMAKE_PROJECT aap_project_name)
	INTERNAL_ADD_CMAKE_PROJECT("${aap_project_name}" "${ARGN}")
ENDMACRO(ADD_CMAKE_PROJECT)

MACRO(ADD_EXTERNAL_LIBRARY ael_library_name)
	INTERNAL_ADD_EXTERNAL_LIBRARY("${ael_library_name}" "${ARGN}")
ENDMACRO(ADD_EXTERNAL_LIBRARY)

INCLUDE(${ACME_PATH}/internal/plugins.cmake)

MACRO(ADD_MODULE ad_module_name ad_type)
	INTERNAL_ADD_MODULE("${ad_module_name}" "${ad_type}")
ENDMACRO(ADD_MODULE)

MACRO(ADD_OPTIONAL_MODULE aom_module_name aom_type)
	INTERNAL_ADD_OPTIONAL_MODULE("${aom_module_name}" "${aom_type}")
ENDMACRO(ADD_OPTIONAL_MODULE)

MACRO(REQUIRED_PACKAGE pkg_name)
	INTERNAL_REQUIRED_PACKAGE("${pkg_name}")
ENDMACRO(REQUIRED_PACKAGE)

MACRO(OPTIONAL_PACKAGE pkg_name)
	INTERNAL_OPTIONAL_PACKAGE("${pkg_name}")
ENDMACRO(OPTIONAL_PACKAGE)

MACRO(ADD_DEPENDENCY ad_name)
	INTERNAL_ADD_DEPENDENCY("${ad_name}")
ENDMACRO(ADD_DEPENDENCY)

MACRO(ADD_COMPILER_FLAG ad_compiler_flag)
	INTERNAL_ADD_COMPILER_FLAG("${ad_compiler_flag}")
ENDMACRO(ADD_COMPILER_FLAG)

MACRO(ADD_DEBUG_COMPILER_FLAG adcf_compiler_flag)
	INTERNAL_ADD_DEBUG_COMPILER_FLAG("${adcf_compiler_flag}")
ENDMACRO(ADD_DEBUG_COMPILER_FLAG)

MACRO(ADD_RELEASE_COMPILER_FLAG arcf_compiler_flag)
	INTERNAL_ADD_RELEASE_COMPILER_FLAG("${arcf_compiler_flag}")
ENDMACRO(ADD_RELEASE_COMPILER_FLAG)

MACRO(ADD_DEFINITION ad_definition)
	INTERNAL_ADD_DEFINITION("${ad_definition}")
ENDMACRO(ADD_DEFINITION)

MACRO(ADD_DEBUG_DEFINITION add_definition)
	INTERNAL_ADD_DEBUG_DEFINITION("${add_definition}")
ENDMACRO(ADD_DEBUG_DEFINITION)

MACRO(ADD_RELEASE_DEFINITION ard_definition)
	INTERNAL_ADD_RELEASE_DEFINITION("${ard_definition}")
ENDMACRO(ADD_RELEASE_DEFINITION)

MACRO(ADD_LINKER_FLAG alf_linker_flag)
	INTERNAL_ADD_LINKER_FLAG("${alf_linker_flag}")
ENDMACRO(ADD_LINKER_FLAG)

MACRO(ADD_DEBUG_LINKER_FLAG adlf_linker_flag)
	INTERNAL_ADD_DEBUG_LINKER_FLAG("${adlf_linker_flag}")
ENDMACRO(ADD_DEBUG_LINKER_FLAG)

MACRO(ADD_RELEASE_LINKER_FLAG arlf_linker_flag)
	INTERNAL_ADD_RELEASE_LINKER_FLAG("${arlf_linker_flag}")
ENDMACRO(ADD_RELEASE_LINKER_FLAG)

MACRO(ADD_FILE)
	INTERNAL_ADD_FILE(${ARGN})
ENDMACRO(ADD_FILE)

MACRO(ADD_OPTIONAL_FILE aoc_name)
	INTERNAL_ADD_OPTIONAL_FILE("${aoc_name}" "${ARGN}")
ENDMACRO(ADD_OPTIONAL_FILE)

MACRO(LINK_LIBRARY ll_name)
	INTERNAL_LINK_LIBRARY("${ll_name}" ${ARGN})
ENDMACRO(LINK_LIBRARY ll_name)

MACRO(LINK_LIBRARY_GROUP llg_name)
	INTERNAL_LINK_LIBRARY_GROUP("${llg_name}" ${ARGN})
ENDMACRO(LINK_LIBRARY_GROUP)

MACRO(ADD_INSTALL_FILE)
	INTERNAL_ADD_INSTALL_FILE("${ARGN}")
ENDMACRO(ADD_INSTALL_FILE)

MACRO(JUST_DOIT)
	INTERNAL_JUST_DOIT()
ENDMACRO(JUST_DOIT)

MACRO(INSTALL_MODULE)
	INTERNAL_INSTALL_MODULE("${ARGN}")
ENDMACRO(INSTALL_MODULE)

MACRO(INSTALL_RESOURCES)
	INTERNAL_INSTALL_RESOURCES("${ARGN}")
ENDMACRO(INSTALL_RESOURCES)

MACRO(FINALIZE)
	INTERNAL_FINALIZE()
ENDMACRO()

MACRO(REPORT)
	INTERNAL_REPORT()
ENDMACRO(REPORT)

