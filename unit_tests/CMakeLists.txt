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

file(GLOB_RECURSE srcs src/*.cpp)

# Organize files into directories
foreach(FILE ${srcs})
   # Get the directory of the source file
   get_filename_component(PARENT_DIR "${FILE}" PATH)

   # Remove common directory prefix to make the group
   string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

if(MSVC)
   # Make sure we are using windows slashes
   string(REPLACE "/" "\\" GROUP "${GROUP}")
endif()

   # Group into "Source Files" and "Header Files"
   if ("${FILE}" MATCHES ".*\\.cpp")
      set(GROUP "Source Files${GROUP}")
   elseif("${FILE}" MATCHES ".*\\.h")
      set(GROUP "Header Files${GROUP}")
   endif()

   source_group("${GROUP}" FILES "${FILE}")
endforeach()

include_directories(../../Ascent)
include_directories(../../ChaiScript/include)

add_executable(${PROJECT_NAME} ${srcs})