message(STATUS "Configuring utfcpp")

get_filename_component(UTFCPP_DIR ${CMAKE_SOURCE_DIR}/deps/utfcpp ABSOLUTE)

# Submodule check
cmutils_directory_is_empty(is_empty "${UTFCPP_DIR}")
if(is_empty)
	message(FATAL_ERROR "utfcpp dependency is missing, maybe you didn't pull the git submodules")
endif()

# Variables
get_filename_component(UTFCPP_INCLUDE_DIR  ${UTFCPP_DIR}/source  ABSOLUTE)
set(UTFCPP_LIBRARY "")

# Message
message("> include: ${UTFCPP_INCLUDE_DIR}")
message(STATUS "Configuring utfcpp - Done")
