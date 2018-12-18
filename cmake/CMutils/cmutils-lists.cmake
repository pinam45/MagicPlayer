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
# - cmutils_filter_list(keep list regexp...)
# - cmutils_filter_out(list regexp...)
# - cmutils_filter_in(list regexp...)
# - cmutils_join_list(output sep items...)

# include guard
if(CMUTILS_LISTS_INCLUDED)
	return()
endif()
set(CMUTILS_LISTS_INCLUDED ON)

# dependencies
include(${CMAKE_CURRENT_LIST_DIR}/cmutils-dev.cmake)

## cmutils_filter_list(keep list regexp...)
# Include/exclude items from a list matching one or more regular expressions.
# Include: keep only the list items that matches regular expressions.
# Exclude: keep only the list items that doesn't matches regular expressions.
#   {value}    [in]     keep:     1 for include, 0 for exclude
#   {variable} [in,out] list:     List to filter
#   {value}    [in]     regexp:   Regular expressions which list items may match
function(cmutils_filter_list _keep _list)
	# variables names are prefixed with '_' to lower chances of parent scope variable hiding
	cmutils_check_variable_name(filter_list ${_list} USED _list _tmp_list)
	set(_tmp_list)
	foreach(_it ${${_list}})
		set(_touch)
		foreach(_regexp ${ARGN})
			if(${_it} MATCHES ${_regexp})
				set(_touch true)
				break()
			endif()
		endforeach()
		if((_keep EQUAL 1 AND DEFINED _touch) OR (_keep EQUAL 0 AND NOT DEFINED _touch))
			list(APPEND _tmp_list ${_it})
		endif()
	endforeach()
	set(${_list} ${_tmp_list} PARENT_SCOPE)
endfunction()

## cmutils_filter_out(list regexp...)
# Exclude items from a list matching one or more regular expressions.
# Exclude: keep only the list items that doesn't matches regular expressions.
#   {variable} [in,out] list:     List to filter
#   {value}    [in]     regexp:   Regular expressions which list items may match
macro(cmutils_filter_out list)
	cmutils_filter_list(0 ${list} ${ARGN})
endmacro()

## cmutils_filter_in(list regexp...)
# Include items from a list matching one or more regular expressions.
# Include: keep only the list items that matches regular expressions.
#   {variable} [in,out] list:     List to filter
#   {value}    [in]     regexp:   Regular expressions which list items may match
macro(cmutils_filter_in list)
	cmutils_filter_list(1 ${list} ${ARGN})
endmacro()

## cmutils_join_list(output sep items...)
# Join items with a separator into a variable.
#   {variable} [out] output:   Output variable, contain the item joined
#   {value}    [in]  sep:      Separator to insert between items
#   {value}    [in]  items:    Items to join
function(cmutils_join_list output sep)
	set(tmp_output)
	set(first)
	foreach(it ${ARGN})
		if(NOT DEFINED first)
			set(tmp_output ${it})
			set(first true)
		else()
			set(tmp_output "${tmp_output}${sep}${it}")
		endif()
	endforeach()
	set(${output} ${tmp_output} PARENT_SCOPE)
endfunction()
