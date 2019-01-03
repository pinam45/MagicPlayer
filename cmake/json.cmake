message(STATUS "Configuring json")

get_filename_component(JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/json ABSOLUTE)

# Submodule check
cmutils_directory_is_empty(is_empty "${JSON_DIR}")
if(is_empty)
	message(FATAL_ERROR "Json dependency is missing, maybe you didn't pull the git submodules")
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${JSON_DIR})

# Variables
set(JSON_INCLUDE_DIR "")
set(JSON_LIBRARY nlohmann_json::nlohmann_json)

# Message
message(STATUS "Configuring json - Done")
