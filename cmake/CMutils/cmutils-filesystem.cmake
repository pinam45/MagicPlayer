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
# - cmutils_directory_is_empty(output dir)
# - cmutils_configure_folder(input_folder output_folder [args...])
# - cmutils_group_files(group root files...)
# - cmutils_get_files(output [RECURSE] directories...)

# include guard
if(CMUTILS_FILESYSTEM_INCLUDED)
	return()
endif()
set(CMUTILS_FILESYSTEM_INCLUDED ON)

# dependencies
include(${CMAKE_CURRENT_LIST_DIR}/cmutils-lists.cmake)

## cmutils_directory_is_empty(output dir)
# Check if a directory is empty.
# If the input directory doesn't exist or is not a directory, it is considered as empty.
#   {variable} [out] output:   true if the directory is empty, false otherwise
#   {value}    [in]  dir:      Directory to check
function(cmutils_directory_is_empty output dir)
	if(ARGC GREATER 2)
		message(FATAL_ERROR "Too many arguments")
	endif()

	set(tmp_output false)
	get_filename_component(dir_path ${dir} REALPATH)
	if(EXISTS "${dir_path}")
		if(IS_DIRECTORY "${dir_path}")
			file(GLOB files "${dir_path}/*")
			list(LENGTH files len)
			if(len EQUAL 0)
				set(tmp_output true)
			endif()
		else()
			set(tmp_output true)
		endif()
	else()
		set(tmp_output true)
	endif()
	set(${output} ${tmp_output} PARENT_SCOPE)
endfunction()

## cmutils_configure_folder(input_folder output_folder [args...])
# Recursively copy all files from an input folder to an output folder
# Copy is made with CMake configure_file(), see documentation for more information:
# https://cmake.org/cmake/help/latest/command/configure_file.html
#   {value} [in] input_folder:    Input folder
#   {value} [in] output_folder:   Output folder
#   {value} [in] args:            CMake configure_file() additional arguments
function(cmutils_configure_folder input_folder output_folder)
	if(NOT EXISTS ${output_folder})
		file(MAKE_DIRECTORY ${output_folder})
	endif()
	file(GLOB_RECURSE files "${input_folder}/*")
	foreach(file ${files})
		file(RELATIVE_PATH relative_file ${input_folder} ${file})
		configure_file(${file} "${output_folder}/${relative_file}" ${ARGN})
	endforeach()
endfunction()

## cmutils_group_files(group root files...)
# Group files in IDE project generation (by calling source_group) relatively to a root.
#   {value} [in] group:   Group, files will be grouped in
#   {value} [in] root:    Root, files will be grouped relative to it
#   {value} [in] files:   Files to group
function(cmutils_group_files group root)
	foreach(it ${ARGN})
		get_filename_component(dir ${it} PATH)
		file(RELATIVE_PATH relative ${root} ${dir})
		set(local ${group})
		if(NOT "${relative}" STREQUAL "")
			set(local "${group}/${relative}")
		endif()
		# replace '/' and '\' (and repetitions) by '\\'
		string(REGEX REPLACE "[\\\\\\/]+" "\\\\\\\\" local ${local})
		source_group("${local}" FILES ${it})
	endforeach()
endfunction()

## cmutils_get_files(output [RECURSE] directories...)
# Get (recursively or not) C and C++ sources files form input directories.
# If an input directory is a file, it is added to output.
#   {variable} [out] output:        Output variable, contain the sources files
#   {value}    [in]  directories:   Directory to search files
#   {option}   [in]  RECURSE:       If present, search is recursive
function(cmutils_get_files output)
	cmake_parse_arguments(ARGS "RECURSE" "" "" ${ARGN})
	if(ARGS_RECURSE)
		set(glob GLOB_RECURSE)
	else()
		set(glob GLOB)
	endif()

	set(extensions .c .C .c++ .cc .cpp .cxx .h .hh .h++ .hpp .hxx .tpp .txx)
	set(files)
	foreach(it ${ARGS_UNPARSED_ARGUMENTS})
		if(IS_DIRECTORY ${it})
			set(patterns)
			foreach(extension ${extensions})
				list(APPEND patterns "${it}/*${extension}")
			endforeach()
			file(${glob} tmp_files ${patterns})
			list(APPEND files ${tmp_files})
		else()
			list(APPEND files ${it})
		endif()
	endforeach()
	set(${output} ${files} PARENT_SCOPE)
endfunction()
