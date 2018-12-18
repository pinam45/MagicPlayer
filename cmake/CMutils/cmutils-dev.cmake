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
# - cmutils_check_variable_name(function_name var_names... USED used_var_names...)
# - cmutils_format_config(config valid_config)
# - cmutils_format_config_inplace(config)
# - cmutils_format_config_uppercase(config valid_config)
# - cmutils_format_config_uppercase_inplace(config)
# - cmutils_target_append_to_property(target property [values...])
# - __cmutils_target_link_flag_property(target flag [configs...])
# - cmutils_define_os_variables()
# - cmutils_define_compiler_variables()
# - cmutils_define_arch_variables()

# include guard
if(CMUTILS_DEV_INCLUDED)
	return()
endif()
set(CMUTILS_DEV_INCLUDED ON)

## cmutils_check_variable_name(function_name var_names... USED used_var_names...)
# Check if variables passed to a function don't have names conflicts with variables used in the function.
# Generate a fatal error on variable name conflict.
#   {value} [in] function_name:    Function name
#   {value} [in] var_names:        Names of the variables passed as parameters of the function
#   {value} [in] used_var_names:   Names of the function internal variables
function(cmutils_check_variable_name _function_name)
	cmake_parse_arguments(ARGS "" "" "USED" ${ARGN})
	foreach(_var_name ${ARGS_UNPARSED_ARGUMENTS})
		foreach(_used_var_name ${ARGS_USED})
			if("${_var_name}" STREQUAL "${_used_var_name}")
				message(FATAL_ERROR "parameter ${_var_name} passed to function ${_function_name} has the same name than an internal variable of the function and can't be accessed")
			endif()
		endforeach()
	endforeach()
endfunction()

## cmutils_format_config(config valid_config)
# Format a config to be cmake-generator-expressions case like: change the case
# to be one of (Debug, RelWithDebInfo, Release).
# If the input config is not valid (not case sensitive), output is empty.
#   {value}    [in]  config:         Config to format
#   {variable} [out] valid_config:   Formated config
function(cmutils_format_config config valid_config)
	if(ARGC GREATER 2)
		message(FATAL_ERROR "Too many arguments")
	endif()

	string(TOLOWER "${config}" config_lower)
	set(config_name)
	foreach(formated_config IN ITEMS "Debug" "RelWithDebInfo" "Release")
		string(TOLOWER "${formated_config}" valid_config_lower)
		if(${config_lower} STREQUAL ${valid_config_lower})
			set(${valid_config} ${formated_config} PARENT_SCOPE)
			return()
		endif()
	endforeach()
	set(${valid_config} "" PARENT_SCOPE)
endfunction()

## cmutils_format_config_inplace(config)
# Format a config to be cmake-generator-expressions case like: change the case
# to be one of (Debug, RelWithDebInfo, Release).
# If the input config is not valid (not case sensitive), output is empty.
#   {variable} [in,out]  config:   Config to format
macro(cmutils_format_config_inplace config)
	cmutils_format_config(${${config}} ${config})
endmacro()

## cmutils_format_config_uppercase(config valid_config)
# Format a config to be <CONFIG> case like: change the case
# to be one of (DEBUG, RELWITHDEBINFO, RELEASE).
# If the input config is not valid (not case sensitive), output is empty.
#   {value}    [in]  config:         Config to format
#   {variable} [out] valid_config:   Formated config
function(cmutils_format_config_uppercase config valid_config)
	if(ARGC GREATER 2)
		message(FATAL_ERROR "Too many arguments")
	endif()

	string(TOUPPER "${config}" config_upper)
	foreach(formated_config IN ITEMS "DEBUG" "RELWITHDEBINFO" "RELEASE")
		if(${config_upper} STREQUAL ${formated_config})
			set(${valid_config} ${formated_config} PARENT_SCOPE)
			return()
		endif()
	endforeach()
	set(${valid_config} "" PARENT_SCOPE)
endfunction()

## cmutils_format_config_uppercase_inplace(config)
# Format a config to be <CONFIG> case like: change the case
# to be one of (DEBUG, RELWITHDEBINFO, RELEASE).
# If the input config is not valid (not case sensitive), output is empty.
#   {variable} [in,out]  config:   Config to format
macro(cmutils_format_config_uppercase_inplace config)
	cmutils_format_config_uppercase(${${config}} ${config})
endmacro()

## cmutils_target_append_to_property(target property [values...])
# Append values to a target property.
#   {value} [in] target:     Target to modify
#   {value} [in] property:   Property to append values to
#   {value} [in] values:     Values to append to the property
function(cmutils_append_to_target_property target property)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()

	set(new_values ${ARGN})
	get_target_property(existing_values ${target} ${property})
	if(existing_values)
		set(new_values "${existing_values} ${new_values}")
	endif()
	set_target_properties(${target} PROPERTIES ${property} ${new_values})
endfunction()

## __cmutils_target_link_flag_property(target flag [configs...])
# Add a flag to the linker arguments of the target for the specified configs using LINK_FLAGS properties of the target.
# Function made for CMake 3.12 or less, future CMake version will have target_link_options() with cmake-generator-expressions.
#   {value} [in] target:    Target to add flag
#   {value} [in] flag:      Flag to add
#   {value} [in] configs:   Configs for the property to change (DEBUG|RELEASE|RELWITHDEBINFO)
function(__cmutils_target_link_flag_property target flag)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(COMMAND target_link_options)
		message(WARNING "Deprecated: function made for CMake 3.12 or less, CMake now have target_link_options() with cmake-generator-expressions support")
	endif()

	if(${ARGC} GREATER 2)
		foreach(config ${ARGN})
			cmutils_format_config_uppercase_inplace(config)
			if(config)
				cmutils_append_to_target_property(${target} LINK_FLAGS_${config} ${flag})
			endif()
		endforeach()
	else()
		cmutils_append_to_target_property(${target} LINK_FLAGS ${flag})
	endif()
endfunction()

## cmutils_define_os_variables()
# Define OS variables to ON/OFF depending on the OS.
# Variables: OS_WINDOWS, OS_UNIX, OS_ANDROID, OS_FREEBSD, OS_IOS, OS_MACOSX, OS_UNKNOW
macro(cmutils_define_os_variables)
	set(OS_WINDOWS OFF)
	set(OS_UNIX OFF)
	set(OS_ANDROID OFF)
	set(OS_LINUX OFF)
	set(OS_FREEBSD OFF)
	set(OS_IOS OFF)
	set(OS_MACOSX OFF)
	set(OS_UNKNOW OFF)
	if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		set(OS_WINDOWS ON)
	elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		set(OS_UNIX ON)
		if(ANDROID)
			set(OS_ANDROID ON)
		else()
			set(OS_LINUX ON)
		endif()
	elseif(CMAKE_SYSTEM_NAME MATCHES "^k?FreeBSD$")
		set(OS_FREEBSD ON)
	elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
		if(IOS)
			set(OS_IOS ON)
		else()
			set(OS_MACOSX ON)
		endif()
	elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
		set(OS_ANDROID ON)
	else()
		set(OS_UNKNOW ON)
	endif()
endmacro()

## cmutils_define_compiler_variables()
# Define compilers variables to ON/OFF depending on the compiler.
# Variables: COMPILER_CLANG, COMPILER_GCC, COMPILER_MSVC, COMPILER_UNKNOW
macro(cmutils_define_compiler_variables)
	set(COMPILER_CLANG OFF)
	set(COMPILER_GCC OFF)
	set(COMPILER_MSVC OFF)
	set(COMPILER_UNKNOW OFF)
	if(CMAKE_CXX_COMPILER MATCHES ".*clang[+][+]" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(COMPILER_CLANG ON)
	elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
		set(COMPILER_GCC ON)
	elseif(MSVC)
		set(COMPILER_MSVC ON)
	else()
		set(COMPILER_UNKNOW ON)
	endif()
endmacro()

## cmutils_define_arch_variables()
# Define architectures variables to ON/OFF depending on the architecture.
# Variables: ARCH_32BITS, ARCH_64BITS, ARCH_UNKNOW
macro(cmutils_define_arch_variables)
	set(ARCH_32BITS OFF)
	set(ARCH_64BITS OFF)
	set(ARCH_UNKNOW OFF)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ARCH_32BITS ON)
	elseif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set(ARCH_64BITS ON)
	else()
		set(ARCH_UNKNOW ON)
	endif()
endmacro()
