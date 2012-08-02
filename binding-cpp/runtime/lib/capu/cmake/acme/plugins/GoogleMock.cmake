
IF(NOT ${BUILD_UNITTESTS})
	UNSET(BUILD_GoogleMock CACHE)
ELSE()
	ADD_CMAKE_PROJECT(	GoogleMock 
						LIBNAMES gmock gtest
						INCLUDE_DIRS "src/GoogleMock/include" "src/GoogleMock/gtest/include" 
						URL "http://googlemock.googlecode.com/files/gmock-1.6.0.zip" 
						CHECKSUM "f547f47321ca88d3965ca2efdcc2a3c1"
						CMAKE_ARGUMENTS "gtest_force_shared_crt:Bool=1" 
						)

ENDIF()

