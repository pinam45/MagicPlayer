message(STATUS "Configuring imgui-sfml")

get_filename_component(IMGUI_DIR ${CMAKE_SOURCE_DIR}/deps/imgui ABSOLUTE)
get_filename_component(IMGUI_SFML_DIR ${CMAKE_SOURCE_DIR}/deps/imgui-sfml ABSOLUTE)
get_filename_component(IMGUI_SFML_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/imgui-sfml ABSOLUTE)

# Submodules check
cmutils_directory_is_empty(is_empty "${IMGUI_DIR}")
if(is_empty)
	message(FATAL_ERROR "ImGui dependency is missing, maybe you didn't pull the git submodules")
endif()
cmutils_directory_is_empty(is_empty "${IMGUI_SFML_DIR}")
if(is_empty)
	message(FATAL_ERROR "imgui-sfml dependency is missing, maybe you didn't pull the git submodules")
endif()

# Configure imgui for imgui-sfml
#if(NOT OPENGL_INCLUDE_DIR OR NOT OPENGL_LIBRARY)
#	message(FATAL_ERROR "Missing OpenGL config")
#endif()
#if(NOT SFML_INCLUDE_DIR OR NOT SFML_LIBRARY)
if(NOT SFML_LIBRARY)
	message(FATAL_ERROR "Missing SFML config")
endif()

# Copy imgui and imgui-sfml files to cmake build folder
cmutils_configure_folder(${IMGUI_DIR} ${IMGUI_SFML_TARGET_DIR} COPYONLY)
cmutils_configure_folder(${IMGUI_SFML_DIR} ${IMGUI_SFML_TARGET_DIR} COPYONLY)
# Include imgui-sfml config header in imgui config header
file(APPEND "${IMGUI_SFML_TARGET_DIR}/imconfig.h"
  "\n#include \"imconfig-SFML.h\"\n"
)

# Declare imgui-sfml
add_library(imgui-sfml STATIC)

# Add sources
cmutils_target_sources_folders(
	imgui-sfml NO_RECURSE PRIVATE
	"${IMGUI_SFML_TARGET_DIR}"
)

# Add includes
target_include_directories(
	imgui-sfml PRIVATE
	"${OPENGL_INCLUDE_DIR}"
	"${SFML_INCLUDE_DIR}"
	"${IMGUI_SFML_TARGET_DIR}"
)

# Link dependencies
target_link_libraries(
	imgui-sfml PRIVATE
	"${SFML_LIBRARY}"
	"${OPENGL_LIBRARY}"
)

# Add definitions
target_compile_definitions(
	imgui-sfml PUBLIC
	IMGUI_DISABLE_OBSOLETE_FUNCTIONS
)

# Configure compile options
cmutils_target_configure_compile_options(imgui-sfml)

# Disable  warnings
cmutils_target_disable_warnings(imgui-sfml)

# Build in C++17
cmutils_target_set_standard(imgui-sfml CXX 17)

# Set dynamic runtime
cmutils_target_set_runtime(imgui-sfml DYNAMIC)

# Set target IDE folder
cmutils_target_set_ide_folder(imgui-sfml "generated/deps")

# Group sources for IDEs
cmutils_target_source_group(imgui-sfml "${IMGUI_SFML_TARGET_DIR}")

# Use ccache
cmutils_target_use_ccache(imgui-sfml)


# Variables
get_filename_component(IMGUI_SFML_INCLUDE_DIR ${IMGUI_SFML_TARGET_DIR} ABSOLUTE)
set(IMGUI_SFML_LIBRARY imgui-sfml)

# Message
message("> include: ${IMGUI_SFML_INCLUDE_DIR}")
message("> library: [compiled with project]")
message(STATUS "Configuring imgui-sfml - Done")
