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

# Split arguments passed to a function into several lists separated by
# specified identifiers that do not have an associated list e.g.:
#
# SET(arguments
#   hello world
#   LIST3 foo bar
#   LIST1 fuz baz
#   )
# ARGUMENT_SPLITTER("${arguments}" "LIST1 LIST2 LIST3" ARG)
#
# results in 8 distinct variables:
#  * ARG_DEFAULT_FOUND: 1
#  * ARG_DEFAULT: hello;world
#  * ARG_LIST1_FOUND: 1
#  * ARG_LIST1: fuz;baz
#  * ARG_LIST2_FOUND: 0
#  * ARG_LIST2:
#  * ARG_LIST3_FOUND: 1
#  * ARG_LIST3: foo;bar

MACRO(INTERNAL_ARGUMENT_SPLITTER argInput argKeywordList argPrefix)

	SET(inputTokenList "DEFAULT ${argInput}")
	SET(keywordList "DEFAULT ${argKeywordList}")
	SET(prefix ${argPrefix})

	# convert provided inputTokenList to list
	SEPARATE_ARGUMENTS(inputTokenList)
	SEPARATE_ARGUMENTS(keywordList)

	# initialize all out variables to default values
	FOREACH(keyword ${keywordList})
		SET(${prefix}_${keyword}_FOUND 0)
		SET(${prefix}_${keyword} "")
	ENDFOREACH(keyword)

	# iterate all tokens of provided input
	FOREACH(token ${inputTokenList})

		# check if current token is a keyword
		SET(tokenIsKeyword 0)
		FOREACH(keyword ${keywordList})
			IF ("${keyword}" STREQUAL "${token}")
				SET(tokenIsKeyword 1)
				SET(lastKeyword ${keyword})
			ENDIF ("${keyword}" STREQUAL "${token}")
		ENDFOREACH(keyword)

		IF (${tokenIsKeyword})
			# if current token is keyword, set found variable to true
			SET(${prefix}_${token}_FOUND 1)
		ELSE (${tokenIsKeyword})
			# if current token is not keyword, append token to variable of last found keyword
			SET(${prefix}_${lastKeyword} ${${prefix}_${lastKeyword}} ${token})
		ENDIF (${tokenIsKeyword})
	ENDFOREACH(token)
ENDMACRO(INTERNAL_ARGUMENT_SPLITTER)

#MACRO(INTERNAL_ADD_OS_SUPPORT os)
#	SET(SUPPORTED_OS ${os} ${ARGN})
#	SET(TARGET_OS ${os} CACHE STRINGS "Select target OS")
#	SET_PROPERTY(CACHE TARGET_OS PROPERTY STRINGS ${SUPPORTED_OS})
#ENDMACRO(INTERNAL_ADD_OS_SUPPORT)


MACRO(INTERNAL_ADD_PREFIX var prefix)
	SET(${var})
	IF(NOT "${ARGN}" STREQUAL "")
	FOREACH(item ${ARGN})
		SET(${var} ${${var}} ${prefix}${item})
	ENDFOREACH(item)
	ENDIF(NOT "${ARGN}" STREQUAL "")
ENDMACRO(INTERNAL_ADD_PREFIX)


MACRO(INTERNAL_SPLIT_PATH_AND_FILE file_with_path path filename)
	GET_FILENAME_COMPONENT(${path} ${file_with_path} PATH)
	IF(NOT "${${path}}" STREQUAL "")
		SET(${path} "${${path}}/")
	ENDIF()
	GET_FILENAME_COMPONENT(${filename} ${file_with_path} NAME)
ENDMACRO(INTERNAL_SPLIT_PATH_AND_FILE)


MACRO(INTERNAL_GET_MODULE_NAME var)
	GET_FILENAME_COMPONENT(${var} ${CMAKE_CURRENT_SOURCE_DIR} NAME)
ENDMACRO(INTERNAL_GET_MODULE_NAME)


MACRO(INTERNAL_GET_PUBLIC_INCLUDE_PATH var)
	INTERNAL_GET_MODULE_NAME(gpip_module_name)
	INTERNAL_GET_INTERN_INCLUDE_PATH(gpip_include_path)	
	SET(${var} "${gpip_include_path}/${gpip_module_name}")
ENDMACRO(INTERNAL_GET_PUBLIC_INCLUDE_PATH)


MACRO(INTERNAL_GET_INTERN_INCLUDE_PATH var)
	SET(${var} ${CMAKE_CURRENT_SOURCE_DIR}/include)
ENDMACRO(INTERNAL_GET_INTERN_INCLUDE_PATH)


MACRO(INTERNAL_GET_PLATFORM_INCLUDE_PATH var gpi_classname)
	INTERNAL_SPLIT_PATH_AND_FILE(${gpi_classname} gpi_path gpi_filename)
	INTERNAL_GET_INTERN_INCLUDE_PATH(gpi_include_path)
	#IF(NOT "${gpi_path}" STREQUAL "")
		SET(${var} "${gpi_include_path}/${CURRENT_MODULE_NAME}/${gpi_path}arch")
	#ELSE()
	#	SET(${var} "${gpi_include_path}/${CURRENT_MODULE_NAME}/arch")
	#ENDIF()
	
ENDMACRO(INTERNAL_GET_PLATFORM_INCLUDE_PATH)


MACRO(INTERNAL_GET_SOURCE_PATH var)
	SET(${var} ${CMAKE_CURRENT_SOURCE_DIR}/src)
ENDMACRO(INTERNAL_GET_SOURCE_PATH)


MACRO(INTERNAL_GET_PLATFORM_SOURCE_PATH var gps_classname)
#	INTERNAL_GET_SOURCE_PATH(gps_source_path)
#	SET(${var} "${gps_source_path}/arch")
	
	INTERNAL_SPLIT_PATH_AND_FILE(${gps_classname} gps_path gps_filename)
	INTERNAL_GET_SOURCE_PATH(gps_source_path)
	SET(${var} "${gps_source_path}/${gps_path}arch")
ENDMACRO(INTERNAL_GET_PLATFORM_SOURCE_PATH)


MACRO(INTERNAL_GET_TEST_PATH var)
	SET(${var} ${CMAKE_CURRENT_SOURCE_DIR}/test)
ENDMACRO(INTERNAL_GET_TEST_PATH)


MACRO(INTERNAL_GET_PATH_TO_FILES gsp_class)
	INTERNAL_ARGUMENT_SPLITTER("${ARGN}" "PREFIX" GSP)
	INTERNAL_SPLIT_PATH_AND_FILE(${gsp_class} GSP_PATH_TO_FILE GSP_FILENAME)
          
	STRING(TOUPPER ${GSP_FILENAME} gsp_varname)
	IF(NOT "${GSP_PREFIX}" STREQUAL "")
		SET(gsp_varname ${GSP_PREFIX})
	#ELSEIF(NOT "${GSP_PLATFORM}" STREQUAL "")
	#	STRING(TOUPPER ${GSP_PLATFORM} gsp_platform_varname)
	#	SET(gsp_varname ${gsp_platform_varname}${gsp_varname})
		
	ENDIF()
                
	SET(gsp_all_classes ${gsp_class} ${GSP_DEFAULT_ARGS})

	SET(${gsp_varname}_PUBLIC_HEADER)
	SET(${gsp_varname}_INTERN_HEADER)
	SET(${gsp_varname}_SOURCE_FILES)
                
	FOREACH(gsp_current_class ${gsp_all_classes})

		INTERNAL_GET_PUBLIC_INCLUDE_PATH( gsp_public_include_path )
                           
		FOREACH(gsp_current_header_type ${HEADER_FILE_EXTENSIONS})
			SET(gsp_public_include_file "${gsp_public_include_path}/${gsp_current_class}.${gsp_current_header_type}" )
			
			IF(EXISTS ${gsp_public_include_file})
				SET(${gsp_varname}_PUBLIC_HEADER ${${gsp_varname}_PUBLIC_HEADER} ${gsp_public_include_file})
			ENDIF()
			
			INTERNAL_GET_PLATFORM_INCLUDE_PATH( gsp_platform_include_path "${gsp_current_class}" )
			SET(gsp_generic_include_file  "${gsp_platform_include_path}/${GSP_FILENAME}.${gsp_current_header_type}" )

			IF(EXISTS ${gsp_generic_include_file})
				SET(${gsp_varname}_PUBLIC_HEADER ${${gsp_varname}_PUBLIC_HEADER} ${gsp_generic_include_file})
			ENDIF()

			INTERNAL_GET_INTERN_INCLUDE_PATH( gsp_intern_include_path )						
			SET(gsp_intern_include_file "${gsp_intern_include_path}/${gsp_current_class}.${gsp_current_header_type}" )
			IF(EXISTS ${gsp_intern_include_file})
				SET(${gsp_varname}_INTERN_HEADER ${${gsp_varname}_INTERN_HEADER} ${gsp_intern_include_file})
			ELSE()
				SET(gsp_intern_include_file "${gsp_platform_include_path}/${TARGET_ARCH}/${GSP_FILENAME}.${gsp_current_header_type}" )
				IF(EXISTS ${gsp_intern_include_file})
					SET(${gsp_varname}_INTERN_HEADER ${${gsp_varname}_INTERN_HEADER} ${gsp_intern_include_file})
				ENDIF()
			ENDIF()
		ENDFOREACH()
               	    
		FOREACH(gsp_current_source_type ${SOURCE_FILE_EXTENSIONS})
                                               
			IF("${TARGET_ARCH}" STREQUAL "")
				INTERNAL_GET_SOURCE_PATH(gsp_source_path)
				SET(gsp_source_file "${gsp_source_path}/${gsp_current_class}.${gsp_current_source_type}" )
			ELSE()
				INTERNAL_GET_PLATFORM_SOURCE_PATH(gsp_platform_source_path "${gsp_current_class}")
				SET(gsp_source_file "${gsp_platform_source_path}/${TARGET_ARCH}/${GSP_FILENAME}.${gsp_current_source_type}" )
			ENDIF()
                                               
			IF(EXISTS ${gsp_source_file})
				SET(${gsp_varname}_SOURCE_FILES ${${gsp_varname}_SOURCE_FILES} ${gsp_source_file})
			ENDIF()

		ENDFOREACH()
		
		INTERNAL_GET_TEST_PATH( gsp_test_path )
		SET(gsp_test_header "${gsp_test_path}/${gsp_current_class}Test.h")
		SET(gsp_test_source "${gsp_test_path}/${gsp_current_class}Test.cpp")
                           					   
		IF(EXISTS ${gsp_test_header})
			SET(${gsp_varname}_TEST_FILES ${${gsp_varname}_TEST_FILES} ${gsp_test_header})
		ENDIF()
                               
		IF(EXISTS ${gsp_test_source})
			SET(${gsp_varname}_TEST_FILES ${${gsp_varname}_TEST_FILES} ${gsp_test_source})
		ENDIF()
                               
	ENDFOREACH()

ENDMACRO(INTERNAL_GET_PATH_TO_FILES)


MACRO(INTERNAL_ADD_CLASS ac_name)
	INTERNAL_ARGUMENT_SPLITTER("${ARGN}" "SOURCE_VAR TEST_VAR SOURCE_GROUP PREFIX CONDITIONS" AC)

	SET(ADD_CLASS_CONDITIONS_MET 1)
	FOREACH(CONDITION ${AC_CONDITIONS})
		IF (NOT ${CONDITION})
			MESSAGE(STATUS "Excluded class ${ac_name}, condition ${CONDITION} failed")
			SET(ADD_CLASS_CONDITIONS_MET 0)
		ENDIF()
	ENDFOREACH()
	
	IF(${ADD_CLASS_CONDITIONS_MET})
		
		SET(ac_all_classes ${ac_name} ${AC_DEFAULT_ARGS})
			
		IF("${AC_PREFIX}" STREQUAL "")
			INTERNAL_SPLIT_PATH_AND_FILE(${ac_name} ac_path ac_filename)
			STRING(TOUPPER ${ac_filename} ac_varname)
			SET(AC_PREFIX ${ac_varname})
		ENDIF()
	
		INTERNAL_GET_PATH_TO_FILES(${ac_all_classes} PREFIX ${AC_PREFIX})
		IF(NOT "${${AC_PREFIX}_SOURCE_FILES}" STREQUAL "")
			SET(${CURRENT_MODULE_NAME}_HAS_SOURCE_FILES 1 CACHE INTERNAL "")
		ENDIF()
		
		IF(NOT "${AC_SOURCE_GROUP}" STREQUAL "")
			SET(${AC_SOURCE_GROUP} ${${AC_SOURCE_GROUP}} ${${AC_PREFIX}_INTERN_HEADER} ${${AC_PREFIX}_SOURCE_FILES})
		ENDIF()
		
		IF(NOT "${AC_SOURCE_VAR}" STREQUAL "")
			SET(${AC_SOURCE_VAR} ${${AC_SOURCE_VAR}} ${${AC_PREFIX}_PUBLIC_HEADER} ${${AC_PREFIX}_INTERN_HEADER} ${${AC_PREFIX}_SOURCE_FILES})
		ELSE()
			SET(${CURRENT_UPPER_MODULE_NAME}_MODULE_SOURCE_FILES ${${CURRENT_UPPER_MODULE_NAME}_MODULE_SOURCE_FILES} ${${AC_PREFIX}_PUBLIC_HEADER} ${${AC_PREFIX}_INTERN_HEADER} ${${AC_PREFIX}_SOURCE_FILES})
		ENDIF()
		
		IF(NOT "${AC_TEST_VAR}" STREQUAL "")
			SET(${AC_TEST_VAR} ${${AC_TEST_VAR}} ${${AC_PREFIX}_TEST_FILES})
		ELSE()
			SET(${CURRENT_MODULE_NAME}_TEST_FILES ${${CURRENT_MODULE_NAME}_TEST_FILES} ${${AC_PREFIX}_TEST_FILES})
		ENDIF()
		
	ENDIF()
	
ENDMACRO(INTERNAL_ADD_CLASS)


MACRO(INTERNAL_ADD_OPTIONAL_CLASS aoc_name)
	INTERNAL_ARGUMENT_SPLITTER("${ARGN}" "SOURCE_VAR SOURCE_GROUP PREFIX CONDITIONS" AOC)
	IF("${AOC_PREFIX}" STREQUAL "")
		INTERNAL_SPLIT_PATH_AND_FILE(${aoc_name} aoc_path aoc_filename)
		STRING(TOUPPER ${aoc_filename} aoc_varname)
		SET(AOC_PREFIX ${aoc_varname})
	ENDIF()

	SET(aoc_temp_files)
	set(aoc_temp_test)
	
	INTERNAL_ADD_CLASS(${aoc_name} ${AOC_DEFAULT_ARGS} SOURCE_VAR aoc_temp_files TEST_VAR aoc_temp_test SOURCE_GROUP ${AOC_SOURCE_GROUP} PREFIX ${AOC_PREFIX} CONDITIONS ${AOC_CONDITIONS})
	INTERNAL_ADD_OPTIONAL_FILES(${CURRENT_UPPER_MODULE_NAME}_${AOC_PREFIX} SOURCE_FILES ${aoc_temp_files} TEST_FILES ${aoc_temp_test})

	SET(${CURRENT_UPPER_MODULE_NAME}_SOURCE_FILES ${${CURRENT_UPPER_MODULE_NAME}_MODULE_SOURCE_FILES} ${${CURRENT_UPPER_MODULE_NAME}_${AOC_PREFIX}_SOURCE_FILES})
	SET(${CURRENT_MODULE_NAME}_TEST_FILES ${${CURRENT_MODULE_NAME}_TEST_FILES} ${${CURRENT_UPPER_MODULE_NAME}_${AOC_PREFIX}_TEST_FILES})
ENDMACRO(INTERNAL_ADD_OPTIONAL_CLASS)


MACRO(INTERNAL_ADD_SOURCE_FILES destination)
	INTERNAL_ARGUMENT_SPLITTER("${ARGN}" "PUBLIC_HEADER INTERN_HEADER SOURCE_FILES" ARG)	
	SET(${destination} ${${destination}} ${ARG_PUBLIC_HEADER} ${ARG_INTERN_HEADER} ${ARG_SOURCE_FILES})
ENDMACRO(INTERNAL_ADD_SOURCE_FILES)


MACRO(INTERNAL_ADD_OPTIONAL_FILES prefix)
	INTERNAL_ARGUMENT_SPLITTER("${ARGN}" "SOURCE_FILES TEST_FILES" AOF)
	
	SET(${prefix}_ENABLE "on" CACHE BOOL "Enable ${prefix}" )
	SET(${prefix}_SOURCE_FILES)
	SET(${prefix}_TEST_FILES)

	IF(${WITH_${CURRENT_UPPER_MODULE_NAME}})
		IF(${${prefix}_ENABLE})
			SET(${prefix}_SOURCE_FILES ${AOF_SOURCE_FILES})
			SET(${prefix}_TEST_FILES   ${AOF_TEST_FILES})
		ENDIF()
	ELSE()
		INTERNAL_REMOVE_OPTIONAL_FILES(${prefix})
	ENDIF()
ENDMACRO(INTERNAL_ADD_OPTIONAL_FILES)


MACRO(INTERNAL_ADD_DEPENDENCY ad_name)
	SET(${CURRENT_MODULE_NAME}_DEPENDENCIES ${${CURRENT_MODULE_NAME}_DEPENDENCIES} ${ad_name})
ENDMACRO(INTERNAL_ADD_DEPENDENCY)


MACRO(INTERNAL_JUST_DOIT)
	IF(${WITH_${CURRENT_UPPER_MODULE_NAME}} AND ${${CURRENT_MODULE_NAME}_BUILD_ENABLED})
	
		INTERNAL_GET_INTERN_INCLUDE_PATH(asl_intern_include_path)
		
		INCLUDE_DIRECTORIES(${asl_intern_include_path})
		
		IF(NOT "${${CURRENT_UPPER_MODULE_NAME}_MODULE_SOURCE_FILES}" STREQUAL "")

			IF(${CURRENT_MODULE_TYPE} STREQUAL "STATIC")
				SET(UTILS_MODULES_STATIC ${UTILS_MODULES_STATIC} ${CURRENT_MODULE_NAME} CACHE INTERNAL "")
				ADD_LIBRARY(${CURRENT_MODULE_NAME}
					STATIC
					${${CURRENT_UPPER_MODULE_NAME}_MODULE_SOURCE_FILES}
				)
			ELSEIF(${CURRENT_MODULE_TYPE} STREQUAL "DYNAMIC")
				SET(UTILS_MODULES_DYNAMIC ${UTILS_MODULES_DYNAMIC} ${CURRENT_MODULE_NAME} CACHE INTERNAL "")
				ADD_LIBRARY(${CURRENT_MODULE_NAME}
					SHARED
					${${CURRENT_UPPER_MODULE_NAME}_MODULE_SOURCE_FILES}
				)
			ELSEIF(${CURRENT_MODULE_TYPE} STREQUAL "EXE")
				SET(UTILS_MODULES_EXE ${UTILS_MODULES_EXE} ${CURRENT_MODULE_NAME} CACHE INTERNAL "")
				ADD_EXECUTABLE(${CURRENT_MODULE_NAME}
					${${CURRENT_UPPER_MODULE_NAME}_MODULE_SOURCE_FILES}
				)
			ENDIF()
		
			INTERNAL_ADD_COMPILER_FLAGS_TO_TARGET(${CURRENT_MODULE_NAME} "${${CURRENT_MODULE_NAME}_COMPILER_FLAGS}")
			INTERNAL_ADD_LINKER_FLAGS_TO_TARGET(${CURRENT_MODULE_NAME} "${${CURRENT_MODULE_NAME}_LINKER_FLAGS}")
			INTERNAL_ADD_DEFINITIONS_TO_TARGET(${CURRENT_MODULE_NAME} "${${CURRENT_MODULE_NAME}_DEFINITIONS}")

			FOREACH(dependency ${${CURRENT_MODULE_NAME}_DEPENDENCIES})
				IF(EXISTS "${PROJECT_SOURCE_DIR}/modules/${dependency}/include")
					INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/modules/${dependency}/include")
					#SET(GLOBAL_TEST_INCLUDE_DIRECTORIES ${GLOBAL_TEST_INCLUDE_DIRECTORIES} "${PROJECT_SOURCE_DIR}/deliverables/include" CACHE INTERNAL "collect test include directories")
					#SET(GLOBAL_TEST_INCLUDE_DIRECTORIES ${GLOBAL_TEST_INCLUDE_DIRECTORIES} "${PROJECT_SOURCE_DIR}/modules/${dependency}/include" CACHE INTERNAL "collect test include directories")
				ELSE()
					MESSAGE(FATAL_ERROR "Added unknown dependency ${dependency}")
				ENDIF()
			ENDFOREACH()

			SET(jd_collected_dependencies ${${CURRENT_MODULE_NAME}_PACKAGE_LIBS} )
			IF(NOT "${${CURRENT_MODULE_NAME}_DEPENDENCIES}" STREQUAL "")
				FOREACH(current_dependency ${${CURRENT_MODULE_NAME}_DEPENDENCIES})
					IF((${${current_dependency}_HAS_SOURCE_FILES}) AND NOT( "${${current_dependency}_MODULE_TYPE}" STREQUAL "EXE"))
						SET(jd_collected_dependencies ${jd_collected_dependencies} ${current_dependency})
					ENDIF()
				ENDFOREACH()
				ADD_DEPENDENCIES(${CURRENT_MODULE_NAME} ${${CURRENT_MODULE_NAME}_DEPENDENCIES})
			ENDIF()

			IF(NOT "${jd_collected_dependencies}" STREQUAL "")
				TARGET_LINK_LIBRARIES(${CURRENT_MODULE_NAME} ${jd_collected_dependencies})
			ENDIF()
			
			SET_TARGET_PROPERTIES(${CURRENT_MODULE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
			
			IF(NOT "${${CURRENT_MODULE_NAME}_TEST_FILES}" STREQUAL "")
				SET(UTILS_MODULES_TESTS ${UTILS_MODULES_TESTS} ${CURRENT_MODULE_NAME}Test CACHE INTERNAL "")

				IF(${BUILD_GLOBAL_TEST_EXECUTABLE})
					#ADD_LIBRARY(${CURRENT_MODULE_NAME}Test STATIC ${${CURRENT_MODULE_NAME}_TEST_FILES})
					#SET(GLOBAL_TEST_INCLUDE_DIRECTORIES ${GLOBAL_TEST_INCLUDE_DIRECTORIES} "${asl_intern_include_path}" CACHE INTERNAL "collect test include directories")
					SET(GLOBAL_TEST_LIBS ${GLOBAL_TEST_LIBS} ${jd_collected_dependencies} CACHE INTERNAL "collect test libs")					
					IF(${${CURRENT_MODULE_NAME}_HAS_SOURCE_FILES})
						SET(GLOBAL_TEST_LIBS ${GLOBAL_TEST_LIBS} ${CURRENT_MODULE_NAME} CACHE INTERNAL "collect test libs")
					ENDIF()
					SET(GLOBAL_TEST_SOURCE ${GLOBAL_TEST_SOURCE} ${${CURRENT_MODULE_NAME}_TEST_FILES} CACHE INTERNAL "collect test source")
					SET(GLOBAL_TEST_COMPILER_FLAGS ${GLOBAL_TEST_COMPILER_FLAGS} ${${CURRENT_MODULE_NAME}_COMPILER_FLAGS} CACHE INTERNAL "collect test compiler flags")
					SET(GLOBAL_TEST_LINKER_FLAGS ${GLOBAL_TEST_LINKER_FLAGS} ${${CURRENT_MODULE_NAME}_LINKER_FLAGS} CACHE INTERNAL "collect test linker flags")
					SET(GLOBAL_TEST_DEFINITIONS ${GLOBAL_TEST_DEFINITIONS} ${${CURRENT_MODULE_NAME}_DEFINITIONS} CACHE INTERNAL "collect test definitions")

				ELSE()
					IF(${${CURRENT_MODULE_NAME}_HAS_SOURCE_FILES})
						SET(jd_collected_dependencies ${jd_collected_dependencies} ${CURRENT_MODULE_NAME})
					ENDIF()
					
					INTERNAL_GET_TEST_PATH(jd_test_path)
					SET(${CURRENT_MODULE_NAME}_TEST_MAIN "")
					IF(EXISTS "${jd_test_path}/main.cpp")
						SET(${CURRENT_MODULE_NAME}_TEST_MAIN "${jd_test_path}/main.cpp")
					ELSE()
						SET(${jd_collected_dependencies} "${jd_collected_dependencies}" gtest_main)
					ENDIF()
					
					LINK_DIRECTORIES(${GTEST_LIB_DIR} ${GMOCK_LIB_DIR})
					ADD_EXECUTABLE(${CURRENT_MODULE_NAME}Test ${${CURRENT_MODULE_NAME}_TEST_FILES} ${${CURRENT_MODULE_NAME}_TEST_MAIN})

					INTERNAL_ADD_COMPILER_FLAGS_TO_TARGET(${CURRENT_MODULE_NAME}Test "${${CURRENT_MODULE_NAME}_COMPILER_FLAGS}")
					INTERNAL_ADD_LINKER_FLAGS_TO_TARGET(${CURRENT_MODULE_NAME}Test "${${CURRENT_MODULE_NAME}_LINKER_FLAGS}")
					INTERNAL_ADD_DEFINITIONS_TO_TARGET(${CURRENT_MODULE_NAME}Test "${${CURRENT_MODULE_NAME}_DEFINITIONS}")

					INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/deliverable/include")
					MESSAGE(STATUS "${CURRENT_MODULE_NAME} contains unit tests, building ${CURRENT_MODULE_NAME}Test")
					
					TARGET_LINK_LIBRARIES(${CURRENT_MODULE_NAME}Test gmock gtest ${jd_collected_dependencies})
					ADD_DEPENDENCIES(${CURRENT_MODULE_NAME}Test GoogleMock GoogleTest ${CURRENT_MODULE_NAME})
					
				ENDIF()

			ENDIF()
			
			# copy header files
			
			ADD_CUSTOM_COMMAND(
				COMMENT "Copy public header files of ${CURRENT_MODULE_NAME} to deliverables."
				POST_BUILD TARGET ${CURRENT_MODULE_NAME}
				COMMAND ${CMAKE_COMMAND} -E make_directory
				     ${CMAKE_HEADER_OUTPUT_DIRECTORY}
				COMMAND ${CMAKE_COMMAND} -E make_directory
				     ${CMAKE_HEADER_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}
				COMMAND ${CMAKE_COMMAND} -E make_directory
				     ${CMAKE_HEADER_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}/${CURRENT_MODULE_NAME}
				COMMAND ${CMAKE_COMMAND} -E copy_directory
				     ${${CURRENT_MODULE_NAME}_DIR}/include/${CURRENT_MODULE_NAME}
				     ${CMAKE_HEADER_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}/${CURRENT_MODULE_NAME}
			)


			# copy resource files
			
			ADD_CUSTOM_COMMAND(
				COMMENT "Copy resource files of ${CURRENT_MODULE_NAME} to deliverables."
				POST_BUILD TARGET ${CURRENT_MODULE_NAME}
				COMMAND ${CMAKE_COMMAND} -E make_directory
				     ${CMAKE_RESOURCE_OUTPUT_DIRECTORY}
				COMMAND ${CMAKE_COMMAND} -E make_directory
				     ${CMAKE_RESOURCE_OUTPUT_DIRECTORY}/${CURRENT_MODULE_NAME}
				COMMAND ${CMAKE_COMMAND} -E copy_directory
				     ${${CURRENT_MODULE_NAME}_DIR}/res
				     ${CMAKE_RESOURCE_OUTPUT_DIRECTORY}/${CURRENT_MODULE_NAME}
			)
			
			# copy documentation files
			
			ADD_CUSTOM_COMMAND(
				COMMENT "Copy documentation files of ${CURRENT_MODULE_NAME} to deliverables."
				POST_BUILD TARGET ${CURRENT_MODULE_NAME}
				COMMAND ${CMAKE_COMMAND} -E make_directory
				     ${CMAKE_DOC_OUTPUT_DIRECTORY}
				COMMAND ${CMAKE_COMMAND} -E make_directory
				     ${CMAKE_DOC_OUTPUT_DIRECTORY}/${CURRENT_MODULE_NAME}
				COMMAND ${CMAKE_COMMAND} -E copy_directory
				     ${${CURRENT_MODULE_NAME}_DIR}/doc
				     ${CMAKE_DOC_OUTPUT_DIRECTORY}/${CURRENT_MODULE_NAME}
			)
			
		ENDIF()
	ENDIF()
ENDMACRO(INTERNAL_JUST_DOIT)

MACRO(INTERNAL_ADD_COMPILER_FLAGS_TO_TARGET acf_target acf_flags)
	SET_TARGET_PROPERTIES(${acf_target} PROPERTIES COMPILE_FLAGS "${acf_flags}" )
ENDMACRO(INTERNAL_ADD_COMPILER_FLAGS_TO_TARGET)

MACRO(INTERNAL_ADD_LINKER_FLAGS_TO_TARGET alf_target alf_flags)
	SET_TARGET_PROPERTIES(${alf_target} PROPERTIES LINK_FLAGS "${alf_flags}" )
ENDMACRO(INTERNAL_ADD_LINKER_FLAGS_TO_TARGET)

MACRO(INTERNAL_ADD_DEFINITIONS_TO_TARGET adt_target adt_definitions)
	SET_TARGET_PROPERTIES(${adt_target} PROPERTIES COMPILE_DEFINITIONS "${adt_definitions}" )
ENDMACRO(INTERNAL_ADD_DEFINITIONS_TO_TARGET)

MACRO(INTERNAL_REPORT_MODULE)
	INTERNAL_ARGUMENT_SPLITTER("${ARGN}" "TYPE MODULES" RM)
	MESSAGE(STATUS)
	MESSAGE("Building ${RM_TYPE} modules:")
	MESSAGE(STATUS)
	FOREACH(r_module ${RM_MODULES})
		MESSAGE(STATUS "${r_module}")
	ENDFOREACH()
	MESSAGE(STATUS)
ENDMACRO(INTERNAL_REPORT_MODULE)


MACRO(INTERNAL_REPORT)
	MESSAGE(STATUS)
	MESSAGE(STATUS "-------------------------------------------------------------------")
	MESSAGE(STATUS "------------------- Configuration report begin --------------------")
	MESSAGE(STATUS "-------------------------------------------------------------------")
	
	IF(NOT "${UTILS_MODULES_STATIC}" STREQUAL "")
		INTERNAL_REPORT_MODULE(TYPE static MODULES ${UTILS_MODULES_STATIC})
		MESSAGE(STATUS "-------------------------------------------------------------------")
	ENDIF()
	
	IF(NOT "${UTILS_MODULES_DYNAMIC}" STREQUAL "")
		INTERNAL_REPORT_MODULE(TYPE dynamic MODULES ${UTILS_MODULES_DYNAMIC})
		MESSAGE(STATUS "-------------------------------------------------------------------")
	ENDIF()
	
	IF(NOT "${UTILS_MODULES_EXE}" STREQUAL "")
		INTERNAL_REPORT_MODULE(TYPE executables MODULES ${UTILS_MODULES_EXE})
		MESSAGE(STATUS "-------------------------------------------------------------------")
	ENDIF()
	
	IF(NOT "${UTILS_MODULES_TESTS}" STREQUAL "")
		INTERNAL_REPORT_MODULE(TYPE tests MODULES ${UTILS_MODULES_TESTS})
		MESSAGE(STATUS "-------------------------------------------------------------------")
	ENDIF()
	MESSAGE(STATUS "------------------- Configuration report end ----------------------")
	MESSAGE(STATUS "-------------------------------------------------------------------")
	MESSAGE(STATUS)
ENDMACRO(INTERNAL_REPORT)


MACRO(INTERNAL_REMOVE_OPTIONAL_FILES prefix)
	UNSET(${prefix}_ENABLE CACHE)
ENDMACRO(INTERNAL_REMOVE_OPTIONAL_FILES)


MACRO(INTERNAL_ADD_MODULE_INTERNAL ami_module_name ami_type)
	MESSAGE("---------------------------------------------------------------------------")
	MESSAGE(STATUS "Configuring build for ${ami_module_name} (${ami_type})")
	STRING(TOUPPER ${ami_type} CURRENT_MODULE_TYPE)
	SET(CURRENT_MODULE_NAME ${ami_module_name})
	STRING(TOUPPER ${CURRENT_MODULE_NAME} CURRENT_UPPER_MODULE_NAME)
	SET(${CURRENT_MODULE_NAME}_HAS_SOURCE_FILES 0 CACHE INTERNAL "")
	SET(${CURRENT_MODULE_NAME}_MODULE_TYPE "${CURRENT_MODULE_TYPE}" CACHE INTERNAL "")
	SET(${CURRENT_MODULE_NAME}_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "")
	SET(${CURRENT_MODULE_NAME}_BUILD_ENABLED 1)
	SET(${CURRENT_MODULE_NAME}_DEFINITIONS   "" CACHE INTERNAL "")
	SET(${CURRENT_MODULE_NAME}_COMPILE_FLAGS "" CACHE INTERNAL "")
	SET(${CURRENT_MODULE_NAME}_LINKER_FLAGS  "" CACHE INTERNAL "")
ENDMACRO(INTERNAL_ADD_MODULE_INTERNAL)


MACRO(INTERNAL_ADD_MODULE ad_module_name ad_type)
	INTERNAL_ADD_MODULE_INTERNAL(${ad_module_name} ${ad_type})
	SET(WITH_${CURRENT_UPPER_MODULE_NAME} 1 CACHE INTERNAL "Use module ${CURRENT_UPPER_MODULE_NAME}")
ENDMACRO(INTERNAL_ADD_MODULE)


MACRO(INTERNAL_ADD_OPTIONAL_MODULE aom_module_name aom_type)
	INTERNAL_ADD_MODULE_INTERNAL(${aom_module_name} ${aom_type})
	SET(WITH_${CURRENT_UPPER_MODULE_NAME} 1 CACHE BOOL "Use module ${CURRENT_UPPER_MODULE_NAME}")
ENDMACRO(INTERNAL_ADD_OPTIONAL_MODULE)


MACRO(INTERNAL_ADD_COMPILER_FLAG ad_compiler_flag)
	SET(${CURRENT_MODULE_NAME}_COMPILER_FLAGS "${${CURRENT_MODULE_NAME}_COMPILER_FLAGS}" ${ad_compiler_flag})
#	SET_TARGET_PROPERTIES(${CURRENT_MODULE_NAME} PROPERTIES COMPILE_FLAGS ${ad_compiler_flag})
#	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ad_compiler_flag}")
#	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ad_compiler_flag}")
#	ADD_RELEASE_COMPILER_FLAG(${ad_compiler_flag})
#	ADD_DEBUG_COMPILER_FLAG(${ad_compiler_flag})
ENDMACRO(INTERNAL_ADD_COMPILER_FLAG)

MACRO(INTERNAL_ADD_DEFINITION ad_definition)
	SET(${CURRENT_MODULE_NAME}_DEFINITIONS "${${CURRENT_MODULE_NAME}_DEFINITIONS}" ${ad_definition})
#	SET_TARGET_PROPERTIES(${CURRENT_MODULE_NAME} PROPERTIES COMPILE_DEFINITIONS ${ad_definition})
ENDMACRO(INTERNAL_ADD_DEFINITION)

#MACRO(INTERNAL_ADD_DEBUG_COMPILER_FLAG adcf_compiler_flag)
#	SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${adcf_compiler_flag}")
#	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${adcf_compiler_flag}")
#ENDMACRO(INTERNAL_ADD_DEBUG_COMPILER_FLAG)


#MACRO(INTERNAL_ADD_RELEASE_COMPILER_FLAG arcf_compiler_flag)
#	SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${arcf_compiler_flag}")
#	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${arcf_compiler_flag}")
#ENDMACRO(INTERNAL_ADD_RELEASE_COMPILER_FLAG)


MACRO(INTERNAL_ADD_LINKER_FLAG ad_linker_flag)
	SET(${CURRENT_MODULE_NAME}_LINKER_FLAGS "${${CURRENT_MODULE_NAME}_LINKER_FLAGS}" ${ad_linker_flag})
#	SET_TARGET_PROPERTIES(${CURRENT_MODULE_NAME} PROPERTIES LINK_FLAGS ${ad_linker_flag})
#	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ad_linker_flag}")
ENDMACRO(INTERNAL_ADD_LINKER_FLAG)


MACRO(INTERNAL_BUILD_UNIT_TESTS)
	IF(${BUILD_GLOBAL_TEST_EXECUTABLE})
		#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/deliverable/bin")
		#ADD_DEBUG_COMPILER_FLAG("/MTd")
		#ADD_RELEASE_COMPILER_FLAG("/MT")
		LINK_DIRECTORIES(${GTEST_LIB_DIR} ${GMOCK_LIB_DIR})
		INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/deliverable/include")
		MESSAGE(STATUS "Global unit test executable enabled, building Test")
		ADD_EXECUTABLE(Test ${GLOBAL_TEST_SOURCE})
		
		INTERNAL_ADD_COMPILER_FLAGS_TO_TARGET(Test "${GLOBAL_TEST_COMPILER_FLAGS}")
		INTERNAL_ADD_LINKER_FLAGS_TO_TARGET(Test "${GLOBAL_TEST_LINKER_FLAGS}")
		INTERNAL_ADD_DEFINITIONS_TO_TARGET(Test "${GLOBAL_TEST_DEFINITIONS}")

		TARGET_LINK_LIBRARIES(Test ${GLOBAL_TEST_LIBS} gmock gtest gtest_main)
		SET_TARGET_PROPERTIES(Test PROPERTIES LINKER_LANGUAGE CXX)
	ENDIF()
ENDMACRO(INTERNAL_BUILD_UNIT_TESTS)


MACRO(INTERNAL_REQUIRED_PACKAGE pkg_name)
	MESSAGE(STATUS "${CURRENT_MODULE_NAME} requires package ${pkg_name}")

	SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
 	
	find_package("${pkg_name}" REQUIRED)
	IF("${pkg_name}_FOUND")
		INCLUDE_DIRECTORIES(${${pkg_name}_INCLUDE_DIR})
		SET(${CURRENT_MODULE_NAME}_PACKAGE_LIBS ${${CURRENT_MODULE_NAME}_PACKAGE_LIBS} ${${pkg_name}_LIBRARIES})
	ELSE("${pkg_name}_FOUND")
		MESSAGE(STATUS "WARNING: Required package '${pkg_name}' was not found.")
		MESSAGE(STATUS "WARNING: Build of '${CURRENT_MODULE_NAME}' was disabled.")
		MESSAGE(STATUS "WARNING: Install package '${pkg_name}' to enable build of '${CURRENT_MODULE_NAME}'.")
		SET(${CURRENT_MODULE_NAME}_BUILD_ENABLED 0)
	ENDIF("${pkg_name}_FOUND")
ENDMACRO(INTERNAL_REQUIRED_PACKAGE)


MACRO(INTERNAL_OPTIONAL_PACKAGE pkg_name)
	MESSAGE(STATUS "${CURRENT_MODULE_NAME} optionally requires package ${pkg_name}")

	SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules/)
	
	find_package(${pkg_name} REQUIRED)
	IF(${pkg_name}_FOUND)
		INCLUDE_DIRECTORIES(${${pkg_name}_INCLUDE_DIR})
		SET(${CURRENT_MODULE_NAME}_PACKAGE_LIBS ${${CURRENT_MODULE_NAME}_PACKAGE_LIBS} ${${pkg_name}_LIBRARIES})
	ELSE(${pkg_name}_FOUND)
		MESSAGE(STATUS "INFO: Optional package '${pkg_name}' was not found.")
	ENDIF(${pkg_name}_FOUND)
ENDMACRO(INTERNAL_OPTIONAL_PACKAGE)


MACRO(INTERNAL_INSTALL_HEADER_FILES ihf_header_file_path)
	INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/modules/${CURRENT_MODULE_NAME}/include/${CURRENT_MODULE_NAME}/
	        DESTINATION ${ihf_header_file_path}
	        FILES_MATCHING
	        PATTERN ".*" EXCLUDE)
ENDMACRO(INTERNAL_INSTALL_HEADER_FILES)


MACRO(INTERNAL_INSTALL_MODULE)

	IF(${WITH_${CURRENT_UPPER_MODULE_NAME}} AND ${${CURRENT_MODULE_NAME}_BUILD_ENABLED})
	
		INTERNAL_ARGUMENT_SPLITTER("${ARGN}" "HEADERS EXECUTABLE LIBRARY" INSTALL_PATH)
	
		IF(${INSTALL_PATH_HEADERS_FOUND})
			MESSAGE(STATUS "Install header files to ${INSTALL_PATH_HEADERS}")
			INTERNAL_INSTALL_HEADER_FILES(${INSTALL_PATH_HEADERS})
		ENDIF(${INSTALL_PATH_HEADERS_FOUND})
	
		IF(${INSTALL_PATH_EXECUTABLE_FOUND})
			MESSAGE(STATUS "Install executable files to ${INSTALL_PATH_EXECUTABLE}")
			SET(INSTALL_COMMAND RUNTIME DESTINATION ${INSTALL_PATH_EXECUTABLE})
		ENDIF(${INSTALL_PATH_EXECUTABLE_FOUND})
	
		IF(${INSTALL_PATH_LIBRARY_FOUND})
			MESSAGE(STATUS "Install library files to ${INSTALL_PATH_LIBRARY}")
			SET(INSTALL_COMMAND ${INSTALL_COMMAND} LIBRARY DESTINATION ${INSTALL_PATH_LIBRARY})
			SET(INSTALL_COMMAND ${INSTALL_COMMAND} ARCHIVE DESTINATION ${INSTALL_PATH_LIBRARY})
		ENDIF(${INSTALL_PATH_LIBRARY_FOUND})
	
		IF(NOT "${INSTALL_COMMAND}" STREQUAL "")
			INSTALL(TARGETS ${CURRENT_MODULE_NAME} LIBRARY DESTINATION ${INSTALL_COMMAND})
		ENDIF(NOT "${INSTALL_COMMAND}" STREQUAL "")
		
	ENDIF()
	
ENDMACRO(INTERNAL_INSTALL_MODULE)


MACRO(INTERNAL_INSTALL_RESOURCES)
	IF(${WITH_${CURRENT_UPPER_MODULE_NAME}} AND ${${CURRENT_MODULE_NAME}_BUILD_ENABLED})
		INTERNAL_ARGUMENT_SPLITTER("${ARGN}" "FILES DESTINATION" RES)
		MESSAGE(STATUS "install resource files to ${RES_DESTINATION}")
		INSTALL(FILES ${RES_FILES} DESTINATION ${RES_DESTINATION})
	ENDIF()
ENDMACRO(INTERNAL_INSTALL_RESOURCES)
