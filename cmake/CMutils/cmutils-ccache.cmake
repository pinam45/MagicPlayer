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
# - cmutils_target_use_ccache(target)

# include guard
if(CMUTILS_CCACHE_INCLUDED)
	return()
endif()
set(CMUTILS_CCACHE_INCLUDED ON)

# dependencies
include(${CMAKE_CURRENT_LIST_DIR}/cmutils-dev.cmake)

## cmutils_target_use_ccache(target)
# Enable ccache use on the given target if the compiler is gcc or clang.
#   {value} [in] target:   Target to configure
function(cmutils_target_use_ccache target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()

	cmutils_define_compiler_variables()
	if(COMPILER_GCC OR COMPILER_CLANG)
		find_program(CCACHE_PROGRAM ccache)
		if(NOT ${CCACHE_PROGRAM} STREQUAL CCACHE_PROGRAM-NOTFOUND)
			message(STATUS "[cmutils] ccache found: ${CCACHE_PROGRAM}")
			set_target_properties(${target} PROPERTIES C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
			set_target_properties(${target} PROPERTIES CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
			message(STATUS "[cmutils] ${target}: enabled ccache use")
		endif()
	endif()
endfunction()
