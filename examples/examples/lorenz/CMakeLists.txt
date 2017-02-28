project(lorenz)
file(GLOB srcs *.cpp *.h)
add_executable(${PROJECT_NAME} ${srcs})