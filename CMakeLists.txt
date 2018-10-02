cmake_minimum_required(VERSION 3.10) # For CXX_STANDARD 17 property on Visual Studio

enable_language(CXX)
#enable_testing()

# By default build in Release mode
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

# Common
include(cmake/common.cmake)

# Config
include(cmake/config.cmake)

# Libraries
include(cmake/opengl.cmake)
include(cmake/SFML.cmake)
include(cmake/imgui.cmake)
include(cmake/imgui-sfml.cmake)
include(cmake/libmpg123.cmake)

# Project
project(MagicPlayer)

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
  INCLUDES "include/"
  EXT_INCLUDES "${OPENGL_INCLUDE_DIR}" "${SFML_INCLUDE_DIR}" "${IMGUI_INCLUDE_DIR}" "${IMGUI_SFML_INCLUDE_DIR}" "${LIBMPG123_INCLUDE_DIR}"
  OPTIONS cxx executable static_runtime
)
target_link_libraries(MagicPlayer PRIVATE ${OPENGL_LIBRARY} ${SFML_LIBRARY} ${IMGUI_SFML_LIBRARY} ${LIBMPG123_LIBRARY})
if(TARGET sfml-main)
	target_link_libraries(MagicPlayer PRIVATE "sfml-main")
	set_target_properties(MagicPlayer PROPERTIES WIN32_EXECUTABLE ON)
endif()
set_target_properties(
  MagicPlayer PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
set_property(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT MagicPlayer
)

#set(CMAKE_VERBOSE_MAKEFILE ON)