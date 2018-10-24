
## target_generate_clang_format(target)
# Generate a format target for the target (format-${target}).
# The generated target lanch clang-format on all the target sources with -style=file
#   {value}  [in] target:   Target from wich generate format target
function(target_generate_clang_format target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "${target} is not a target)")
	endif()

	set(format-target "format-${target}")
	find_program(CLANG_FORMAT clang-format
	  NAMES clang-format-9 clang-format-8 clang-format-7 clang-format-6)
	if(${CLANG_FORMAT} STREQUAL CLANG_FORMAT-NOTFOUND)
		message(WARNING "clang-format not found, ${format-target} not generated")
		return()
	else()
		message(STATUS "clang-format found: ${CLANG_FORMAT}")
	endif()

	get_property(target_sources TARGET ${target} PROPERTY SOURCES)
	add_custom_target(
	  ${format-target}
	  COMMAND "${CLANG_FORMAT}" -style=file -i ${target_sources}
	  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	  VERBATIM
	)
	message(STATUS "Format target ${format-target} generated")
endfunction()
