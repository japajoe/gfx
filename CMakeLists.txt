cmake_minimum_required(VERSION 3.15)
project(gfx)

set(CMAKE_CXX_STANDARD 20)

# GLFW settings
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

# miniaudioex settings
set(MINIAUDIOEX_BUILD_SHARED OFF CACHE BOOL "Build miniaudioex shared library")
if(UNIX AND NOT APPLE)
	set(MINIAUDIOEX_BUILD_PLATFORM "linux_x86_64")
elseif(APPLE)
	set(MINIAUDIOEX_BUILD_PLATFORM "mac")
elseif(WIN32)
	set(MINIAUDIOEX_BUILD_PLATFORM "windows")
endif()

# freetype settings
set(FT_DISABLE_ZLIB ON CACHE BOOL "Disable use of system zlib and use internal zlib library instead")
set(FT_DISABLE_BZIP2 ON CACHE BOOL "Disable support of bzip2 compressed fonts")
set(FT_DISABLE_PNG ON CACHE BOOL "Disable support of PNG compressed OpenType embedded bitmaps")
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable HarfBuzz (used for improving auto-hinting of OpenType fonts)")
set(FT_DISABLE_BROTLI ON CACHE BOOL "Disable support of compressed WOFF2 fonts")

# glm settings
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "Build glm as dynamic/static library")

# assimp settings
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build our own zlib")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "If the test suite for Assimp is built in addition to the library")
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Build Assimp with all exporters enabled")
set(ASSIMP_INSTALL OFF CACHE BOOL "Install Assimp library. Disable this if you want to use Assimp as a submodule")
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "Install MSVC debug files")
set(USE_STATIC_CRT ON CACHE BOOL "Link against the static MSVC runtime libraries")
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "Build Assimp view tool (requires DirectX)")

add_subdirectory(libs/glfw)
add_subdirectory(libs/miniaudioex)
add_subdirectory(libs/freetype)
add_subdirectory(libs/glm)
add_subdirectory(libs/assimp)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-DNDEBUG)
endif()

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/include/Audio"
	"${PROJECT_SOURCE_DIR}/include/Audio/DSP"
	"${PROJECT_SOURCE_DIR}/include/Core"
	"${PROJECT_SOURCE_DIR}/include/Embedded"
	"${PROJECT_SOURCE_DIR}/include/External/assimp"
	"${PROJECT_SOURCE_DIR}/include/External/glad"
	"${PROJECT_SOURCE_DIR}/include/External/glfw"
	"${PROJECT_SOURCE_DIR}/include/External/imgui"
	"${PROJECT_SOURCE_DIR}/include/External/miniaudioex"
	"${PROJECT_SOURCE_DIR}/include/Graphics"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Buffers"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Shaders"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Materials"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Renderers"
	"${PROJECT_SOURCE_DIR}/include/System"
	"${PROJECT_SOURCE_DIR}/include/System/Collections"
	"${PROJECT_SOURCE_DIR}/include/System/Numerics"
)

add_definitions(-DNDEBUG)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw miniaudioex freetype glm assimp)