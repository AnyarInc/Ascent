cmake_minimum_required(VERSION 3.18)
project("ascent"
		VERSION 0.5.0
		DESCRIPTION "An extremely fast and flexible C++ simulation engine and differential equation solver."
		HOMEPAGE_URL "https://github.com/AnyarInc/Ascent"
		LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CPack)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
if (MSVC)
	target_compile_options(${PROJECT_NAME} INTERFACE "/bigobj") # for ChaiScript
endif()

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/catch2>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
	add_subdirectory(unit_tests)
endif()

option(BUILD_EXAMPLES "Build example projects" TRUE)
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()