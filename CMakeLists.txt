cmake_minimum_required(VERSION 3.15)
project(gfx)

set(CMAKE_CXX_STANDARD 20)

# Add GLFW as a submodule

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

add_subdirectory(libs/glfw)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/include/external/glad"
	"${PROJECT_SOURCE_DIR}/include/external/glfw"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)