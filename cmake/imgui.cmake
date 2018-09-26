message(STATUS "Configuring imgui")

get_filename_component(IMGUI_DIR ${CMAKE_SOURCE_DIR}/deps/imgui ABSOLUTE)

# Submodule check
directory_is_empty(is_empty "${IMGUI_DIR}")
if(is_empty)
	message(FATAL_ERROR "ImGui dependency is missing, maybe you didn't pull the git submodules")
endif()

# Variables
get_filename_component(IMGUI_INCLUDE_DIR  ${IMGUI_DIR}  ABSOLUTE)
set(IMGUI_LIBRARY "")

# Message
message("> include: ${IMGUI_INCLUDE_DIR}")
message(STATUS "Configuring imgui - Done")
