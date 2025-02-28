cmake_minimum_required(VERSION 3.15)
project(gfx)

set(CMAKE_CXX_STANDARD 20)

# Windows dependant setting
set(GFX_STATIC_MSVC_RUNTIME OFF CACHE BOOL "Link against the static MSVC runtime libraries")

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

# assimp settings
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build our own zlib")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "If the test suite for Assimp is built in addition to the library")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "Build Assimp with all importers enabled")
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Build Assimp with all exporters enabled")
set(ASSIMP_BUILD_COLLADA_IMPORTER ON BOOL)
set(ASSIMP_BUILD_OBJ_IMPORTER ON BOOL)
set(ASSIMP_BUILD_BLEND_IMPORTER ON BOOL)
set(ASSIMP_BUILD_FBX_IMPORTER ON BOOL)
set(ASSIMP_BUILD_GLTF_IMPORTER ON BOOL)
set(ASSIMP_INSTALL OFF CACHE BOOL "Install Assimp library. Disable this if you want to use Assimp as a submodule")
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "Install MSVC debug files")
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "Build Assimp view tool (requires DirectX)")

if(GFX_STATIC_MSVC_RUNTIME)
	set(USE_STATIC_CRT ON CACHE BOOL "Link against the static MSVC runtime libraries")
elseif()
	set(USE_STATIC_CRT OFF CACHE BOOL "Link against the static MSVC runtime libraries")
endif()

# JoltPhysics settings
set(JPH_SHARED_LIBRARY OFF CACHE BOOL "Use the Jolt library as a shared library. Use JPH_BUILD_SHARED_LIBRARY to build Jolt as a shared library.")
set(JPH_DOUBLE_PRECISION ON CACHE BOOL "Compiles the library so that all positions are stored in doubles instead of floats. This makes larger worlds possible.")
set(JPH_USE_STD_VECTOR ON CACHE BOOL "Use std::vector instead of Jolt's own Array class.")
set(JPH_PROFILE_ENABLED ON CACHE BOOL "Turns on the internal profiler.")
set(JPH_ENABLE_ASSERTS ON CACHE BOOL "Compiles the library so that it rises an assert in case of failures. The library ignores these failures otherwise.")
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "Use the static MSVC runtime library.")

if(GFX_STATIC_MSVC_RUNTIME)
	set(USE_STATIC_MSVC_RUNTIME_LIBRARY ON CACHE BOOL "Use the static MSVC runtime library.")
elseif()
	set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "Use the static MSVC runtime library.")
endif()

add_subdirectory(libs/glfw)
add_subdirectory(libs/miniaudioex)
add_subdirectory(libs/freetype)
add_subdirectory(libs/assimp)
add_subdirectory(libs/JoltPhysics/Build)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-DNDEBUG)
else()
	add_definitions(-DGFX_DEBUG_LOGGING)
endif()

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/include/Audio"
	"${PROJECT_SOURCE_DIR}/include/Audio/DSP"
	"${PROJECT_SOURCE_DIR}/include/Core"
	"${PROJECT_SOURCE_DIR}/include/Embedded"
	"${PROJECT_SOURCE_DIR}/include/External/glad"
	"${PROJECT_SOURCE_DIR}/include/External/glm"
	"${PROJECT_SOURCE_DIR}/include/External/glm/detail"
	"${PROJECT_SOURCE_DIR}/include/External/glm/ext"
	"${PROJECT_SOURCE_DIR}/include/External/glm/gtc"
	"${PROJECT_SOURCE_DIR}/include/External/glm/gtx"
	"${PROJECT_SOURCE_DIR}/include/External/glm/simd"
	"${PROJECT_SOURCE_DIR}/include/External/imgui"
	"${PROJECT_SOURCE_DIR}/include/Graphics"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Buffers"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Shaders"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Shaders/PostProcessing"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Materials"
	"${PROJECT_SOURCE_DIR}/include/Graphics/Renderers"
	"${PROJECT_SOURCE_DIR}/include/Physics"
	"${PROJECT_SOURCE_DIR}/include/Physics/Collision"
	"${PROJECT_SOURCE_DIR}/include/Physics/Joints"
	"${PROJECT_SOURCE_DIR}/include/System"
	"${PROJECT_SOURCE_DIR}/include/System/Collections"
	"${PROJECT_SOURCE_DIR}/include/System/IO"
	"${PROJECT_SOURCE_DIR}/include/System/Numerics"
	"${PROJECT_SOURCE_DIR}/include/Testing"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw miniaudioex freetype assimp Jolt)
target_include_directories(${PROJECT_NAME} PRIVATE libs/JoltPhysics/Jolt)