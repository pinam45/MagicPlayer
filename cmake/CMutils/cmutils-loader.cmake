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
# - cmutils_load_flags(prefix file_path...)
# - cmutils_load_default_flags(prefix)

# include guard
if(CMUTILS_LOADER_INCLUDED)
	return()
endif()
set(CMUTILS_LOADER_INCLUDED ON)

# dependencies
include(${CMAKE_CURRENT_LIST_DIR}/json-cmake/JSONParser.cmake)

# global variables
set(CMUTILS_DEFAULT_FLAGS_PATH "${CMAKE_CURRENT_LIST_DIR}/default-flags.json")

## cmutils_load_flags(prefix file_path...)
# Load flags from JSON files, see default-flags.json for an example.
# Create variables ${prefix}_<compiler>_<lang>_FLAGS containing flags.
# Compilers values: MSVC, GCC, CLANG
# Lang values: COMMON, C, CXX
#   {value} [in] prefix:      Prefix for generated variables
#   {value} [in] file_path:   JSON files containing flags
function(cmutils_load_flags prefix)
	if(NOT DEFINED ARGN)
		message(FATAL_ERROR "Missing arguments: files paths")
	endif()

	set(output_vars)
	foreach(file_path ${ARGN})
		if(NOT EXISTS "${file_path}")
			message(FATAL_ERROR "Invalid flags file (doesn't exist or can't be read): ${file_path}")
		endif()

		file(READ ${file_path} file_content)
		sbeParseJson(flags file_content)

		foreach(compiler IN ITEMS msvc gcc clang)
			foreach(group IN ITEMS common c c++)
				string(TOUPPER ${compiler} compiler_upper)
				string(TOUPPER ${group} group_upper)
				string(REGEX REPLACE "[+]" "X" group_upper ${group_upper})
				set(json_var flags.${compiler}.${group})
				set(output_var ${prefix}_${compiler_upper}_${group_upper}_FLAGS)
				list(APPEND output_vars ${output_var})


				set(tmp_list)
				foreach(count in ${${json_var}})
					list(APPEND tmp_list ${${json_var}_${count}})
				endforeach()
				set(${output_var}_TMP ${${output_var_tmp}_TMP} ${tmp_list})
			endforeach()
		endforeach()
	endforeach()

	foreach(output_var ${output_vars})
		if(${output_var}_TMP)
			list(REMOVE_DUPLICATES ${output_var}_TMP)
		endif()
		set(${output_var} ${${output_var}_TMP} PARENT_SCOPE)
	endforeach()
endfunction()

## cmutils_load_default_flags(prefix)
# Load default flags from default-flags.json.
# Create variables ${prefix}_<compiler>_<lang>_FLAGS containing flags.
# Compilers values: MSVC, GCC, CLANG
# Lang values: COMMON, C, CXX
#   {value} [in] prefix:   Prefix for generated variables
macro(cmutils_load_default_flags prefix)
	set(default_flags_path "${CMUTILS_DEFAULT_FLAGS_PATH}")
	if(EXISTS "${default_flags_path}")
		cmutils_load_flags(${prefix} "${default_flags_path}")
	else()
		message(FATAL_ERROR "Missing default flags files: ${default_flags_path}")
	endif()
endmacro()
