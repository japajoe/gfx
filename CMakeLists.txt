cmake_minimum_required(VERSION 3.15)
project(demo)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(gfx)

file(GLOB_RECURSE SOURCES demo/src/*.cpp demo/src/*.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/demo/include/"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE gfx)