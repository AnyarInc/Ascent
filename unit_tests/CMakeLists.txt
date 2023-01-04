include(FetchContent)

FetchContent_Declare(
  ut
  GIT_REPOSITORY https://github.com/boost-ext/ut.git
  GIT_TAG tags/v1.1.9
  GIT_SHALLOW TRUE
  FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(ut)

add_executable(${PROJECT_NAME}_test src/main.cpp)
target_compile_definitions(${PROJECT_NAME}_test PUBLIC "BOOST_UT_ENABLE_RUN_AFTER_BUILD OFF" "BOOST_UT_DISABLE_MODULE ON")
target_link_libraries(${PROJECT_NAME}_test PRIVATE Boost::ut ascent)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)