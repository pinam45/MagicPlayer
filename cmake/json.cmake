message(STATUS "Configuring json")

get_filename_component(JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/json ABSOLUTE)

# Submodule check
cmutils_directory_is_empty(is_empty "${JSON_DIR}")
if(is_empty)
	message(FATAL_ERROR "Json dependency is missing, maybe you didn't pull the git submodules")
endif()

# Include json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${JSON_DIR})
if(NOT TARGET nlohmann_json)
	message(FATAL_ERROR "nlohmann_json target is missing")
endif()

# Disable warnings on nlohmann_json headers
get_target_property(nlohmann_json_include_directories nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
set_target_properties(nlohmann_json PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
target_include_directories(nlohmann_json SYSTEM INTERFACE ${nlohmann_json_include_directories})

message(STATUS "Configuring json - Done")
