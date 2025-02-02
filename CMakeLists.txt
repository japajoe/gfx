cmake_minimum_required(VERSION 3.15)
project(gfx)

set(CMAKE_CXX_STANDARD 20)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

set(MINIAUDIOEX_BUILD_SHARED OFF CACHE BOOL "Build miniaudioex shared library")

set(FT_REQUIRE_ZLIB OFF CACHE BOOL "Require system zlib instead of internal zlib library")
set(FT_REQUIRE_BZIP2 OFF CACHE BOOL "Disable support of bzip2 compressed fonts")
set(FT_REQUIRE_PNG OFF CACHE BOOL "Disable support of PNG compressed OpenType embedded bitmaps")
set(FT_REQUIRE_HARFBUZZ OFF CACHE BOOL "Disable HarfBuzz (used for improving auto-hinting of OpenType fonts)")
set(FT_REQUIRE_BROTLI OFF CACHE BOOL "Disable support of compressed WOFF2 fonts")

set(GLM_BUILD_LIBRARY OFF CACHE BOOL "Build glm as dynamic/static library")

add_subdirectory(libs/glfw)
add_subdirectory(libs/miniaudioex)
add_subdirectory(libs/freetype)
add_subdirectory(libs/glm)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/include/Core"
	"${PROJECT_SOURCE_DIR}/include/External/glad"
	"${PROJECT_SOURCE_DIR}/include/External/glfw"
	"${PROJECT_SOURCE_DIR}/include/External/imgui"
	"${PROJECT_SOURCE_DIR}/include/Graphics"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Buffers"
	"${PROJECT_SOURCE_DIR}/include/System/Numerics"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw miniaudioex freetype glm)