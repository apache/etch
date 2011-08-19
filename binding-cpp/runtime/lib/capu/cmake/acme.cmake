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

INCLUDE(${CMAKE_TOOLCHAIN_FILE})
INCLUDE(cmake/internal/config.cmake)
INCLUDE(cmake/internal/externalProjects.cmake)
INCLUDE(cmake/internal/macros.cmake)

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

MACRO(ADD_DEFINITION ad_definition)
	INTERNAL_ADD_DEFINITION("${ad_definition}")
ENDMACRO(ADD_DEFINITION)

#MACRO(ADD_DEBUG_COMPILER_FLAG adcf_compiler_flag)
#	INTERNAL_ADD_DEBUG_COMPILER_FLAG("${adcf_compiler_flag}")
#ENDMACRO(ADD_DEBUG_COMPILER_FLAG)

#MACRO(ADD_RELEASE_COMPILER_FLAG arcf_compiler_flag)
#	INTERNAL_ADD_RELEASE_COMPILER_FLAG("${arcf_compiler_flag}")
#ENDMACRO(ADD_RELEASE_COMPILER_FLAG)

MACRO(ADD_LINKER_FLAG ad_linker_flag)
	INTERNAL_ADD_LINKER_FLAG("${ad_linker_flag}")
ENDMACRO(ADD_LINKER_FLAG)

MACRO(ADD_CLASS)
	INTERNAL_ADD_CLASS(${ARGN})
ENDMACRO(ADD_CLASS)

MACRO(ADD_OPTIONAL_CLASS aoc_name)
	INTERNAL_ADD_OPTIONAL_CLASS("${aoc_name}" "${ARGN}")
ENDMACRO(ADD_OPTIONAL_CLASS)

MACRO(JUST_DOIT)
	INTERNAL_JUST_DOIT()
ENDMACRO(JUST_DOIT)

MACRO(INSTALL_MODULE)
	INTERNAL_INSTALL_MODULE("${ARGN}")
ENDMACRO(INSTALL_MODULE)

MACRO(INSTALL_RESOURCES)
	INTERNAL_INSTALL_RESOURCES("${ARGN}")
ENDMACRO(INSTALL_RESOURCES)

MACRO(BUILD_UNIT_TESTS)
	INTERNAL_BUILD_UNIT_TESTS()
ENDMACRO()

MACRO(REPORT)
	INTERNAL_REPORT()
ENDMACRO(REPORT)

