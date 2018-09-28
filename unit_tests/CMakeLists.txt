cmake_minimum_required(VERSION 3.0)
project(ascent_unit_tests)

if(MSVC)
	# for ChaiScript
	add_definitions(/bigobj)
else()
	set(DEBUG_FLAGS "-std=c++14")
	set(RELEASE_FLAGS "-std=c++14")

	set(CMAKE_CXX_FLAGS ${RELEASE_FLAGS})
	set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
endif()

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ascent)