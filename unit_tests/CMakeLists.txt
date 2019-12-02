add_executable(${PROJECT_NAME}_test src/main.cpp)
target_link_libraries(${PROJECT_NAME}_test ascent)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)