cmake_minimum_required(VERSION 3.0)
project(ascent)

# ascent library
add_library(ascent INTERFACE)
target_include_directories(ascent INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)