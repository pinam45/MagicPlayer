message(STATUS "Configuring utfcpp")

get_filename_component(UTFCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/utfcpp ABSOLUTE)

# Submodule check
cmutils_directory_is_empty(is_empty "${UTFCPP_DIR}")
if(is_empty)
	message(FATAL_ERROR "utfcpp dependency is missing, maybe you didn't pull the git submodules")
endif()

# Include utfcpp
set(UTF8_TESTS OFF CACHE INTERNAL "")
set(UTF8_SAMPLES OFF CACHE INTERNAL "")
add_subdirectory(${UTFCPP_DIR})
if(NOT TARGET utf8cpp)
	message(FATAL_ERROR "utf8cpp target is missing")
endif()

# Disable warnings on utf8cpp headers
get_target_property(utf8cpp_include_directories utf8cpp INTERFACE_INCLUDE_DIRECTORIES)
set_target_properties(utf8cpp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
target_include_directories(utf8cpp SYSTEM INTERFACE ${utf8cpp_include_directories})

message(STATUS "Configuring utfcpp - Done")
