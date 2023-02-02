enable_language(CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Done in developer mode only, so users won't be bothered by this :)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/ascent/*.hpp")
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX headers FILES ${headers})

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
source_group(TREE "${PROJECT_SOURCE_DIR}/src" PREFIX sources FILES ${sources})

add_executable(ascent_ide ${sources} ${headers})

target_link_libraries(ascent_ide PRIVATE ascent::ascent)

set_target_properties(ascent_ascent ascent_ide PROPERTIES FOLDER ProjectTargets)
