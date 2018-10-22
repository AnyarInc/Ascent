cmake_minimum_required(VERSION 3.0)
project(ascent)

if(MSVC)
	# for ChaiScript
	add_definitions(/bigobj)
else()
	set(DEBUG_FLAGS "-std=c++14")
	set(RELEASE_FLAGS "-std=c++14")

	set(CMAKE_CXX_FLAGS ${RELEASE_FLAGS})
	set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
endif()

## Put all binary files into /bin and libraries into /lib
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ascent library
add_library(ascent INTERFACE)
target_include_directories(ascent INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)
	
option(BUILD_EXAMPLES "Build example projects" TRUE)
option(BUILD_UNIT_TESTS "Build unit testing project" TRUE)

if(BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()

if (BUILD_UNIT_TESTS)
	add_subdirectory(unit_tests)
endif()