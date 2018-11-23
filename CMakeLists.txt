cmake_minimum_required(VERSION 3.10) # For CXX_STANDARD 17 property on Visual Studio

# Project declaration
project(
	MagicPlayer
	VERSION 1.0.1
	DESCRIPTION "The magic music player"
	HOMEPAGE_URL "https://github.com/pinam45/MagicPlayer"
	LANGUAGES CXX
)

#enable_language(CXX)
#enable_testing()

# By default build in Release mode
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

# Common
include(cmake/common.cmake)

# Config
include(cmake/config.cmake)

# clang-format
include(cmake/clang-format.cmake)

# Libraries
include(cmake/opengl.cmake)
include(cmake/SFML.cmake)
include(cmake/imgui-sfml.cmake)
include(cmake/libmpg123.cmake)
include(cmake/spdlog.cmake)
include(cmake/IconFontCppHeaders.cmake)
include(cmake/utfcpp.cmake)

# Add postfix in debug mode
#set(CMAKE_DEBUG_POSTFIX -d)

# Setup MagicPlayer
get_files(
  files
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/include"
  OPTIONS recurse
)
make_target(
  MagicPlayer "MagicPlayer" ${files}
  INCLUDES
    "${CMAKE_SOURCE_DIR}/include"
  EXT_INCLUDES
    "${OPENGL_INCLUDE_DIR}"
    "${SFML_INCLUDE_DIR}"
    "${IMGUI_INCLUDE_DIR}"
    "${IMGUI_SFML_INCLUDE_DIR}"
    "${LIBMPG123_INCLUDE_DIR}"
    "${SPDLOG_INCLUDE_DIR}"
    "${ICON_FONT_CPP_HEADERS_INCLUDE_DIR}"
	"${UTFCPP_INCLUDE_DIR}"
  OPTIONS
    cxx
    executable
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
)

# WinMain config
if(TARGET sfml-main)
	target_link_libraries(MagicPlayer PRIVATE "sfml-main")
	set_target_properties(MagicPlayer PROPERTIES WIN32_EXECUTABLE ON)
endif()

# Link threads library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(MagicPlayer PRIVATE Threads::Threads)

# Link filesystem library for clang and gcc < 9.0
if(CONFIG_COMPILER_CLANG OR (CMAKE_COMPILER_IS_GNUCXX AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)))
	target_link_libraries(MagicPlayer PRIVATE stdc++fs)
endif()

# Build in C++17
set_target_properties(
  MagicPlayer PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# Visual studio config
set_target_properties(
  MagicPlayer PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
set_property(
  DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT MagicPlayer
)

# Resources
configure_folder(
  "${CMAKE_SOURCE_DIR}/resources"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources"
  COPYONLY
)

# Enable console log in debug
target_add_compile_definition(MagicPlayer ENABLE_CONSOLE_LOG)

# Enable debug / trace logs
target_add_compile_definition(MagicPlayer SPDLOG_DEBUG_ON RELWITHDEBINFO DEBUG)
#target_add_compile_definition(MagicPlayer SPDLOG_TRACE_ON DEBUG)

# Generate format target
target_generate_clang_format(MagicPlayer)

# Verbose makefile
#set(CMAKE_VERBOSE_MAKEFILE ON)
