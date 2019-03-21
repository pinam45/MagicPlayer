cmake_minimum_required(VERSION 3.10) # For CXX_STANDARD 17 property on Visual Studio

# Project declaration
project(
	MagicPlayer
	VERSION 1.0.1
	DESCRIPTION "The magic music player"
	HOMEPAGE_URL "https://github.com/pinam45/MagicPlayer"
	LANGUAGES CXX C
)

# By default build in Release mode
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type: Release, RelWithDebInfo or Debug" FORCE)
endif()

# CMake utilities
get_filename_component(CMUTILS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMutils/cmutils.cmake" ABSOLUTE)
if(EXISTS "${CMUTILS_PATH}")
	include(${CMUTILS_PATH})
else()
	message(FATAL_ERROR "CMutils is missing, maybe you didn't pull the git submodules")
endif()

# Global config
cmutils_disable_in_sources_build()
cmutils_enable_ide_folders()
cmutils_set_classic_output_folders()

# Libraries
include(cmake/opengl.cmake)
include(cmake/SFML.cmake)
include(cmake/imgui-sfml.cmake)
include(cmake/libmpg123.cmake)
include(cmake/spdlog.cmake)
include(cmake/IconFontCppHeaders.cmake)
include(cmake/utfcpp.cmake)
include(cmake/taglib.cmake)
include(cmake/json.cmake)

# Declare MagicPlayer
add_executable(MagicPlayer)

# Add sources
cmutils_target_sources_folders(
	MagicPlayer PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/include"
)

# Add includes
target_include_directories(
	MagicPlayer PRIVATE
	"${CMAKE_SOURCE_DIR}/include"
)

# Add external includes
target_include_directories(
	MagicPlayer SYSTEM PRIVATE
	"${OPENGL_INCLUDE_DIR}"
	"${SFML_INCLUDE_DIR}"
	"${IMGUI_INCLUDE_DIR}"
	"${IMGUI_SFML_INCLUDE_DIR}"
	"${LIBMPG123_INCLUDE_DIR}"
	"${SPDLOG_INCLUDE_DIR}"
	"${ICON_FONT_CPP_HEADERS_INCLUDE_DIR}"
	"${UTFCPP_INCLUDE_DIR}"
	"${TAGLIB_INCLUDE_DIR}"
	"${JSON_INCLUDE_DIR}"
)

# Link dependencies
target_link_libraries(
  MagicPlayer PRIVATE
  ${OPENGL_LIBRARY}
  ${SFML_LIBRARY}
  ${IMGUI_SFML_LIBRARY}
  ${LIBMPG123_LIBRARY}
  ${SPDLOG_LIBRARY}
  ${ICON_FONT_CPP_HEADERS_LIBRARY}
  ${UTFCPP_LIBRARY}
  ${TAGLIB_LIBRARY}
  ${JSON_LIBRARY}
)

# Link threads library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(MagicPlayer PRIVATE Threads::Threads)

# Link filesystem library for clang and gcc < 9.0
cmutils_define_compiler_variables()
if(COMPILER_CLANG OR (COMPILER_GCC AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)))
	target_link_libraries(MagicPlayer PRIVATE stdc++fs)
endif()

# Configure compile options
cmutils_target_configure_compile_options(MagicPlayer)

# Enable cmutils default warnings
cmutils_target_enable_warnings(MagicPlayer)

# Build in C++17
cmutils_target_set_standard(MagicPlayer CXX 17)

# Set static runtime
cmutils_target_set_runtime(MagicPlayer STATIC)

# Enable sanitizers in debug
#cmutils_target_enable_sanitizers(
#	MagicPlayer
#	ADDRESS LEAK UNDEFINED MEMORY
#	DEBUG
#)

# WinMain config
if(TARGET sfml-main)
	target_link_libraries(MagicPlayer PRIVATE "sfml-main")
	set_target_properties(MagicPlayer PROPERTIES WIN32_EXECUTABLE ON)
endif()

# Set target IDE folder
cmutils_target_set_ide_folder(MagicPlayer "MagicPlayer")

# Group sources for IDEs
cmutils_target_source_group(MagicPlayer "${CMAKE_SOURCE_DIR}")

# Visual studio config
get_target_property(magicplayer_binary_folder MagicPlayer RUNTIME_OUTPUT_DIRECTORY)
set_target_properties(
  MagicPlayer PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${magicplayer_binary_folder}"
)
set_property(
  DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT MagicPlayer
)

# Resources
cmutils_configure_folder(
  "${CMAKE_SOURCE_DIR}/resources"
  "${magicplayer_binary_folder}/resources"
  COPYONLY
)

# Enable console log in debug
cmutils_target_add_compile_definition(MagicPlayer ENABLE_CONSOLE_LOG)

# Enable debug / trace logs
cmutils_target_add_compile_definition(MagicPlayer SPDLOG_DEBUG_ON RELWITHDEBINFO DEBUG)
#cmutils_target_add_compile_definition(MagicPlayer SPDLOG_TRACE_ON DEBUG)

# Use ccache
cmutils_target_use_ccache(MagicPlayer)

# Generate format target
cmutils_target_generate_clang_format(MagicPlayer)

# Verbose makefile
#set(CMAKE_VERBOSE_MAKEFILE ON)
