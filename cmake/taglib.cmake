message(STATUS "Configuring taglib")

get_filename_component(TAGLIB_DIR "${CMAKE_SOURCE_DIR}/deps/taglib" ABSOLUTE)
get_filename_component(TAGLIB_SRC_DIR "${TAGLIB_DIR}/taglib" ABSOLUTE)
get_filename_component(TAGLIB_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/deps/taglib/config" ABSOLUTE)
get_filename_component(TAGLIB_IMPORTED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/deps/taglib/include" ABSOLUTE)

# Submodule check
cmutils_directory_is_empty(is_empty "${TAGLIB_DIR}")
if(is_empty)
	message(FATAL_ERROR "taglib dependency is missing, maybe you didn't pull the git submodules")
endif()

# tag declaration: taget to build taglib
add_library(tag STATIC)

# Compiler features and ZLIB installation checks
include(${TAGLIB_DIR}/ConfigureChecks.cmake)
if(ZLIB_FOUND)
	target_include_directories(tag PRIVATE ${ZLIB_INCLUDE_DIR})
	target_link_libraries(tag PUBLIC ${ZLIB_LIBRARIES})
endif()

# Configuration headers
set(TRACE_IN_RELEASE FALSE)
configure_file(${TAGLIB_DIR}/config.h.cmake "${TAGLIB_CONFIG_DIR}/config.h")
configure_file(${TAGLIB_DIR}/taglib/taglib_config.h.cmake "${TAGLIB_CONFIG_DIR}/taglib_config.h")
configure_file(${TAGLIB_DIR}/taglib/taglib_config.h.cmake "${TAGLIB_IMPORTED_INCLUDE_DIR}/taglib/taglib_config.h")

# Add sources
cmutils_target_sources_folders(
	tag PRIVATE
	"${TAGLIB_SRC_DIR}"
)

# Add include directories: main folders
target_include_directories(
	tag PRIVATE
	"${TAGLIB_CONFIG_DIR}"
	"${TAGLIB_SRC_DIR}"
	"${TAGLIB_DIR}/3rdparty"
)

# Add include directories: ${TAGLIB_CONFIG_DIR} sub-folders
file(GLOB folders_list "${TAGLIB_SRC_DIR}/*")
list(LENGTH folders_list folders_list_length)
while(folders_list_length GREATER 0)
	list(GET folders_list 0 folder)
	list(REMOVE_AT folders_list 0)
	if(IS_DIRECTORY "${folder}")
		target_include_directories(tag PRIVATE "${folder}")
		file(GLOB sub_folders_list "${folder}/*")
		list(APPEND folders_list ${sub_folders_list})
	endif()
	list(LENGTH folders_list folders_list_length)
endwhile()

# Add definitions
target_compile_definitions(tag PRIVATE TAGLIB_STATIC)
target_compile_definitions(tag PRIVATE HAVE_CONFIG_H)

# Configure compile options
cmutils_target_configure_compile_options(tag)

# Disable  warnings
cmutils_target_disable_warnings(tag)

# Build in C++11
cmutils_target_set_standard(tag CXX 11)

# Set dynamic runtime
cmutils_target_set_runtime(tag DYNAMIC)

# Set target IDE folder
cmutils_target_set_ide_folder(tag "deps/taglib")

# Group sources for IDEs
cmutils_target_source_group(tag "${TAGLIB_SRC_DIR}")

# Use ccache
cmutils_target_use_ccache(tag)


# taglib declaration: taget to link taglib
add_library(taglib INTERFACE)
target_link_libraries(taglib INTERFACE tag)
cmutils_get_headers(taglib_headers RECURSE ${TAGLIB_SRC_DIR})
file(
	COPY ${taglib_headers}
	DESTINATION "${TAGLIB_IMPORTED_INCLUDE_DIR}/taglib"
	PATTERN "*/tagutils.h" EXCLUDE
)
target_include_directories(taglib SYSTEM INTERFACE "${TAGLIB_IMPORTED_INCLUDE_DIR}")

# Variables
set(TAGLIB_INCLUDE_DIR "")
set(TAGLIB_LIBRARY taglib)

# Message
message("> include: ${TAGLIB_IMPORTED_INCLUDE_DIR}/taglib")
message("> library: [compiled with project]")
message(STATUS "Configuring taglib - Done")
