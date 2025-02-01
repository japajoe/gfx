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

add_subdirectory(libs/glfw)
add_subdirectory(libs/miniaudioex)
add_subdirectory(libs/freetype)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/include/external/glad"
	"${PROJECT_SOURCE_DIR}/include/external/glfw"
	"${PROJECT_SOURCE_DIR}/include/external/imgui"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw miniaudioex freetype)