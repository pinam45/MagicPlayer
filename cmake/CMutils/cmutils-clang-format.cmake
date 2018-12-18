##################################################################################
# MIT License                                                                    #
#                                                                                #
# Copyright (c) 2018 Maxime Pinard                                               #
#                                                                                #
# Permission is hereby granted, free of charge, to any person obtaining a copy   #
# of this software and associated documentation files (the "Software"), to deal  #
# in the Software without restriction, including without limitation the rights   #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      #
# copies of the Software, and to permit persons to whom the Software is          #
# furnished to do so, subject to the following conditions:                       #
#                                                                                #
# The above copyright notice and this permission notice shall be included in all #
# copies or substantial portions of the Software.                                #
#                                                                                #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  #
# SOFTWARE.                                                                      #
##################################################################################

# Functions summary:
# - cmutils_target_generate_clang_format(target [STYLE style] [WORKING_DIRECTORY work_dir])

# include guard
if(CMUTILS_CLANG_FORMAT_INCLUDED)
	return()
endif()
set(CMUTILS_CLANG_FORMAT_INCLUDED ON)

# dependencies
include(${CMAKE_CURRENT_LIST_DIR}/cmutils-lists.cmake)

## cmutils_target_generate_clang_format(target [STYLE style] [WORKING_DIRECTORY work_dir])
# Generate a format target for the target (format-${target}). The generated target lanch
# clang-format on all the target sources with the specified style (or the 'file' style by default)
# in the specified working directory (${CMAKE_CURRENT_SOURCE_DIR} by default}).
# With 'file' style the working directory should contain a ".clang-format" file.
#   {value} [in] target:     Target from wich generate format target
#   {value} [in] style:      clang-format style (file, LLVM, Google, Chromium, Mozilla, WebKit)
#   {value} [in] work_dir:   clang-format working directory
function(cmutils_target_generate_clang_format target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	cmake_parse_arguments(ARGS "" "STYLE;WORKING_DIRECTORY" "" ${ARGN})
	if(ARGS_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Invalid arguments: ${ARGS_UNPARSED_ARGUMENTS}")
	endif()
	if(NOT ARGS_STYLE)
		set(ARGS_STYLE file)
	endif()
	if(NOT ARGS_WORKING_DIRECTORY)
		set(ARGS_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
	endif()

	set(format-target "format-${target}")
	find_program(CLANG_FORMAT clang-format
		NAMES clang-format-9 clang-format-8 clang-format-7 clang-format-6)
	if(${CLANG_FORMAT} STREQUAL CLANG_FORMAT-NOTFOUND)
		message(WARNING "[cmutils] clang-format not found, ${format-target} not generated")
		return()
	else()
		message(STATUS "[cmutils] clang-format found: ${CLANG_FORMAT}")
	endif()

	get_property(target_sources TARGET ${target} PROPERTY SOURCES)
	add_custom_target(
		${format-target}
		COMMAND "${CLANG_FORMAT}" -style=${ARGS_STYLE} -i ${target_sources}
		WORKING_DIRECTORY "${ARGS_WORKING_DIRECTORY}"
		VERBATIM
	)
	cmutils_target_set_ide_folder(${format-target} "format")
	message(STATUS "[cmutils] ${target}: Generated format target ${format-target}")
endfunction()
