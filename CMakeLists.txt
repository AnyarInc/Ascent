cmake_minimum_required(VERSION 3.0)
project(ascent_project)

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

add_subdirectory(ascent)
add_subdirectory(examples)
add_subdirectory(unit_tests)