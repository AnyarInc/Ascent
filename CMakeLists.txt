cmake_minimum_required(VERSION 3.18)

include(cmake/prelude.cmake)

project(ascent
        VERSION 1.0.0
        DESCRIPTION "An extremely fast and flexible C++ simulation engine and differential equation solver."
        HOMEPAGE_URL "https://github.com/AnyarInc/Ascent"
        LANGUAGES CXX)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

add_library(ascent_ascent INTERFACE)
add_library(ascent::ascent ALIAS ascent_ascent)

set_property(TARGET ascent_ascent PROPERTY EXPORT_NAME ascent)

target_compile_features(ascent_ascent INTERFACE cxx_std_20)
target_include_directories(
        ascent_ascent ${warning_guard}
    INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(cmake/dev-mode.cmake)
  add_subdirectory(examples)
endif()