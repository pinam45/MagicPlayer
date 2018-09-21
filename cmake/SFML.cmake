message(STATUS "Configuring SFML")

get_filename_component(SFML_DIR ${CMAKE_SOURCE_DIR}/deps/SFML ABSOLUTE)

# Submodule check
directory_is_empty(is_empty "${SFML_DIR}")
if(is_empty)
	message(FATAL_ERROR "SFML dependency is missing, maybe you didn't pull the git submodules")
endif()

# Subproject
add_subdirectory(${SFML_DIR})

# Setup targets output, put exe and required SFML dll in the same folder
target_set_output_directory(sfml-system "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
target_set_output_directory(sfml-window "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
target_set_output_directory(sfml-graphics "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
target_set_output_directory(sfml-audio "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Configure SFML folder in IDE
foreach(sfml_target IN ITEMS sfml-system sfml-network sfml-window sfml-graphics sfml-audio sfml-main)
	if(TARGET ${sfml_target})
		set_target_properties(${sfml_target} PROPERTIES FOLDER deps/SFML)
	endif()
endforeach()

# Variables
get_filename_component(SFML_INCLUDE_DIR  ${SFML_DIR}/include  ABSOLUTE)
set(SFML_LIBRARY sfml-system sfml-network sfml-window sfml-graphics sfml-audio)

# Message
message("> include: ${SFML_INCLUDE_DIR}")
message("> library: [compiled with project]")
message(STATUS "Configuring SFML - Done")
