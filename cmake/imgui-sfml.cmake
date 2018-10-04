message(STATUS "Configuring imgui-sfml")

get_filename_component(IMGUI_SFML_DIR ${CMAKE_SOURCE_DIR}/deps/imgui-sfml ABSOLUTE)

# Submodule check
directory_is_empty(is_empty "${IMGUI_SFML_DIR}")
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
if(NOT IMGUI_DIR OR NOT IMGUI_INCLUDE_DIR)
	message(FATAL_ERROR "Missing imgui config")
endif()

message(STATUS "imgui configuration for SFML")
if(NOT EXISTS "${IMGUI_DIR}/CMake_imgui_SFML_Config_Done")
	set(tmp ${CMAKE_DISABLE_SOURCE_CHANGES})
	set(CMAKE_DISABLE_SOURCE_CHANGES OFF)
	file(READ "${IMGUI_SFML_DIR}/imconfig-SFML.h" imconfig-SFML_content)
	file(APPEND "${IMGUI_DIR}/imconfig.h" "${imconfig-SFML_content}")
	file(WRITE "${IMGUI_DIR}/CMake_imgui_SFML_Config_Done" "")
	message(STATUS "imgui configuration for SFML - Done")
	set(CMAKE_DISABLE_SOURCE_CHANGES ${tmp})
else()
	message(STATUS "imgui configuration for SFML - Already done")
endif()

# Setup target
get_files(
  files
  "${IMGUI_DIR}"
  "${IMGUI_SFML_DIR}"
)
make_target(
  imgui-sfml "deps/imgui-sfml" ${files}
  INCLUDES "${OPENGL_INCLUDE_DIR}" "${SFML_INCLUDE_DIR}" "${IMGUI_INCLUDE_DIR}" "${IMGUI_SFML_DIR}"
  OPTIONS cxx no_warnings
)
target_link_libraries(imgui-sfml PRIVATE "${SFML_LIBRARY}" "${OPENGL_LIBRARY}")
target_compile_definitions(imgui-sfml PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

# Variables
get_filename_component(IMGUI_SFML_INCLUDE_DIR  ${IMGUI_SFML_DIR}  ABSOLUTE)
set(IMGUI_SFML_LIBRARY imgui-sfml)

# Message
message("> include: ${IMGUI_SFML_INCLUDE_DIR}")
message("> library: [compiled with project]")
message(STATUS "Configuring imgui-sfml - Done")
