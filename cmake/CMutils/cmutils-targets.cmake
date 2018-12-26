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
# - cmutils_target_sources_folders(target [NO_RECURSE] [INTERFACE interface_folders...] [PUBLIC public_folders...] [PRIVATE private_folders...])
# - cmutils_target_add_compile_definition(target definition [configs...])
# - cmutils_target_add_compiler_flag(target lang flag [configs...])
# - cmutils_target_add_c_compiler_flag(target flag [configs...])
# - cmutils_target_add_cxx_compiler_flag(target flag [configs...])
# - cmutils_target_add_linker_flag(target lang flag [configs...])
# - cmutils_target_add_c_linker_flag(target flag [configs...])
# - cmutils_target_add_cxx_linker_flag(target flag [configs...])
# - cmutils_target_set_standard(target target [C c_std] [CXX cxx_std])
# - cmutils_target_set_output_directory(target [RUNTIME runtime_directory] [LIBRARY library_directory] [ARCHIVE archive_directory])
# - cmutils_target_set_output_directory(target directory)
# - cmutils_target_set_classic_output_directories(target directory)
# - cmutils_target_set_runtime_output_directory(target directory)
# - cmutils_target_set_library_output_directory(target directory)
# - cmutils_target_set_archive_output_directory(target directory)
# - cmutils_target_configure_msvc_compile_options(target)
# - cmutils_target_configure_gcc_compile_options(target)
# - cmutils_target_configure_clang_compile_options(target)
# - cmutils_target_configure_compile_options(target)
# - cmutils_target_set_runtime(target <STATIC|DYNAMIC>)
# - cmutils_target_enable_sanitizers(target sanitizers... [configs...])
# - cmutils_target_enable_warnings(target [json_flags_files...])
# - cmutils_target_disable_warnings(target)
# - cmutils_target_set_ide_folder(target folder)
# - cmutils_target_source_group(target root)

# include guard
if(CMUTILS_TARGETS_INCLUDED)
	return()
endif()
set(CMUTILS_TARGETS_INCLUDED ON)

# dependencies
include(${CMAKE_CURRENT_LIST_DIR}/cmutils-dev.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmutils-filesystem.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmutils-loader.cmake)

## cmutils_target_sources_folders(target [NO_RECURSE] [INTERFACE interface_folders...]
#                                       [PUBLIC public_folders...] [PRIVATE private_folders...])
# Add sources from the specified folders with the specified scope to the specified target.
# Only C/C++ sources files will be considered.
#   {value}  [in] target:              Target to configure
#   {option} [in] NO_RECURSE:          If present, folder are not explored recursively
#   {value}  [in] interface_folders:   Source folders with INTERFACE scope
#   {value}  [in] public_folders:      Source folders with PUBLIC scope
#   {value}  [in] private_folders:     Source folders with PRIVATE scope
function(cmutils_target_sources_folders target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	cmake_parse_arguments(ARGS "NO_RECURSE" "" "INTERFACE;PUBLIC;PRIVATE" ${ARGN})
	if(ARGS_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Invalid arguments: ${ARGS_UNPARSED_ARGUMENTS}")
	endif()

	foreach(scope IN ITEMS INTERFACE PUBLIC PRIVATE)
		if(ARGS_${scope})
			if(ARGS_NO_RECURSE)
				cmutils_get_sources(files ${ARGS_${scope}})
			else()
				cmutils_get_sources(files RECURSE ${ARGS_${scope}})
			endif()
			target_sources(${target} ${scope} ${files})
		endif()
	endforeach()
endfunction()

## cmutils_target_add_compile_definition(target definition [configs...])
# Add a private compile definition to the target for the specified configs.
#   {value} [in] target:       Target to add flag
#   {value} [in] definition:   Definition to add
#   {value} [in] configs:      Configs for the property to change (DEBUG|RELEASE|RELWITHDEBINFO)
function(cmutils_target_add_compile_definition target definition)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 2)
		foreach(config ${ARGN})
			cmutils_format_config_inplace(config)
			if(config)
				target_compile_definitions(${target} PRIVATE "$<$<CONFIG:${config}>:${definition}>")
			endif()
		endforeach()
	else()
		target_compile_definitions(${target} PRIVATE "${definition}")
	endif()
endfunction()

## cmutils_target_add_compiler_flag(target lang flag [configs...])
# Add a flag to the compiler arguments of the target for the specified language and configs.
# Add the flag only if the compiler support it (checked with CHECK_(C|CXX)_COMPILER_FLAG).
#   {value} [in] target:    Target to add flag
#   {value} [in] lang:      Language of the flag (C|CXX)
#   {value} [in] flag:      Flag to add
#   {value} [in] configs:   Configs for the property to change (DEBUG|RELEASE|RELWITHDEBINFO)
function(cmutils_target_add_compiler_flag target lang flag)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	string(TOUPPER ${lang} lang)
	string(REGEX REPLACE "[+]" "X" lang ${lang})
	if(lang STREQUAL "C")
		include(CheckCCompilerFlag)
		CHECK_C_COMPILER_FLAG(${flag} has${flag})
	elseif(lang STREQUAL "CXX")
		include(CheckCXXCompilerFlag)
		CHECK_CXX_COMPILER_FLAG(${flag} has${flag})
	else()
		message(WARNING "[cmutils] Unsuported language: ${lang}, compiler flag ${flag} not added")
		return()
	endif()

	if(has${flag})
		if(${ARGC} GREATER 3)
			foreach(config ${ARGN})
				cmutils_format_config_inplace(config)
				if(config)
					target_compile_options(${target} PRIVATE "$<$<AND:$<COMPILE_LANGUAGE:${lang}>,$<CONFIG:${config}>>:${flag}>")
				endif()
			endforeach()
		else()
			target_compile_options(${target} PRIVATE "$<$<COMPILE_LANGUAGE:${lang}>:${flag}>")
		endif()
	endif()
endfunction()

## cmutils_target_add_c_compiler_flag(target flag [configs...])
# Add a flag to the C compiler arguments of the target for the specified configs.
# Add the flag only if the compiler support it (checked with CHECK_C_COMPILER_FLAG).
#   {value} [in] target:    Target to add flag
#   {value} [in] flag:      Flag to add
#   {value} [in] configs:   Configs for the property to change (DEBUG|RELEASE|RELWITHDEBINFO)
macro(cmutils_target_add_c_compiler_flag target flag)
	cmutils_target_add_compiler_flag(${target} C ${flag} ${ARGN})
endmacro()

## cmutils_target_add_cxx_compiler_flag(target flag [configs...])
# Add a flag to the C++ compiler arguments of the target for the specified configs.
# Add the flag only if the compiler support it (checked with CHECK_CXX_COMPILER_FLAG).
#   {value} [in] target:    Target to add flag
#   {value} [in] flag:      Flag to add
#   {value} [in] configs:   Configs for the property to change (DEBUG|RELEASE|RELWITHDEBINFO)
macro(cmutils_target_add_cxx_compiler_flag target flag)
	cmutils_target_add_compiler_flag(${target} CXX ${flag} ${ARGN})
endmacro()

## cmutils_target_add_linker_flag(target lang flag [configs...])
# Add a flag to the linker arguments of the target for the specified language and configs.
# Add the flag only if the linker support it (checked with CHECK_(C|CXX)_COMPILER_FLAG).
#   {value} [in] target:    Target to add flag
#   {value} [in] lang:      Language of the flag (C|CXX)
#   {value} [in] flag:      Flag to add
#   {value} [in] configs:   Configs for the property to change (DEBUG|RELEASE|RELWITHDEBINFO)
function(cmutils_target_add_linker_flag target lang flag)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	string(TOUPPER ${lang} lang)
	string(REGEX REPLACE "[+]" "X" lang ${lang})

	# Check linker flags with CHECK_<LANG>_COMPILER_FLAG as there is at the moment no
	# utility to check linker flags
	if(lang STREQUAL "C")
		include(CheckCCompilerFlag)
		CHECK_C_COMPILER_FLAG(${flag} has${flag})
	elseif(lang STREQUAL "CXX")
		include(CheckCXXCompilerFlag)
		CHECK_CXX_COMPILER_FLAG(${flag} has${flag})
	else()
		message(WARNING "[cmutils] Unsuported language: ${lang}, linker flag ${flag} not added")
		return()
	endif()

	if(has${flag})
		if(${ARGC} GREATER 3)
			foreach(config ${ARGN})
				if(COMMAND target_link_options)
					cmutils_format_config_inplace(config)
					if(config)
						target_link_options(${target} PRIVATE "$<$<AND:$<COMPILE_LANGUAGE:${lang}>,$<CONFIG:${config}>>:${flag}>")
					endif()
				else()
					# old CMake version: use ugly workaround
					cmutils_format_config_uppercase_inplace(config)
					if(config)
						__cmutils_target_link_flag_property(${target} ${flag} ${config})
					endif()
				endif()
			endforeach()
		else()
			if(COMMAND target_link_options)
				target_link_options(${target} PRIVATE "$<$<COMPILE_LANGUAGE:${lang}>:${flag}>")
			else()
				# old CMake version: use ugly workaround
				__cmutils_target_link_flag_property(${target} "${flag}")
			endif()
		endif()
	endif()
endfunction()

## cmutils_target_add_c_linker_flag(target flag [configs...])
# Add a flag to the C linker arguments of the target for the specified configs.
# Add the flag only if the linker support it (checked with CHECK_C_COMPILER_FLAG).
#   {value} [in] target:    Target to add flag
#   {value} [in] flag:      Flag to add
#   {value} [in] configs:   Configs for the property to change (DEBUG|RELEASE|RELWITHDEBINFO)
macro(cmutils_target_add_c_linker_flag target flag)
	cmutils_target_add_linker_flag(${target} C ${flag} ${ARGN})
endmacro()

## cmutils_target_add_cxx_linker_flag(target flag [configs...])
# Add a flag to the C++ linker arguments of the target for the specified configs.
# Add the flag only if the linker support it (checked with CHECK_CXX_COMPILER_FLAG).
#   {value} [in] target:    Target to add flag
#   {value} [in] flag:      Flag to add
#   {value} [in] configs:   Configs for the property to change (DEBUG|RELEASE|RELWITHDEBINFO)
macro(cmutils_target_add_cxx_linker_flag target flag)
	cmutils_target_add_linker_flag(${target} CXX ${flag} ${ARGN})
endmacro()

## cmutils_target_set_standard(target target [C c_std] [CXX cxx_std])
# Set the target language standard to use, also set the standard as required and disable compiler extentions.
#   {value} [in] target:    Target to configure
#   {value} [in] c_std:     C standard to use (90|99|11)
#   {value} [in] cxx_std:   CXX standard to use (98|11|14|17|20)
function(cmutils_target_set_standard target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	set(LANGS C CXX)
	cmake_parse_arguments(ARGS "" "${LANGS}" "" ${ARGN})
	if(ARGS_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Invalid arguments: ${ARGS_UNPARSED_ARGUMENTS}")
	endif()

	foreach(lang ${LANGS})
		if(DEFINED ARGS_${lang})
			string(REGEX REPLACE "X" "+" print_lang ${lang})
			message(STATUS "[cmutils] ${target}: ${print_lang} standard set to ${ARGS_${lang}}")
			set_target_properties(
				${target} PROPERTIES
				${lang}_STANDARD ${ARGS_${lang}}
				${lang}_STANDARD_REQUIRED ON
				${lang}_EXTENSIONS OFF
			)
		endif()
	endforeach()
endfunction()

## cmutils_target_set_output_directory(target [RUNTIME runtime_directory] [LIBRARY library_directory] [ARCHIVE archive_directory])
## cmutils_target_set_output_directory(target directory)
# Set the target runtime, library and archive output directory.
#   {value} [in] target:              Target to configure
#   {value} [in] runtime_directory:   Runtime output directory
#   {value} [in] library_directory:   Library output directory
#   {value} [in] archive_directory:   Archive output directory
#   {value} [in] directory:           Runtime, library and archive output directory
function(cmutils_target_set_output_directory target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	cmake_parse_arguments(ARGS "" "RUNTIME;LIBRARY;ARCHIVE" "" ${ARGN})
	if(NOT (ARGS_RUNTIME OR ARGS_LIBRARY OR ARGS_ARCHIVE))
		if(${ARGC} GREATER 2)
			message(FATAL_ERROR "Invalid arguments")
		endif()
		set(ARGS_RUNTIME ${ARGS_UNPARSED_ARGUMENTS})
		set(ARGS_LIBRARY ${ARGS_UNPARSED_ARGUMENTS})
		set(ARGS_ARCHIVE ${ARGS_UNPARSED_ARGUMENTS})
	else()
		if(ARGS_UNPARSED_ARGUMENTS)
			message(FATAL_ERROR "Invalid arguments: ${ARGS_UNPARSED_ARGUMENTS}")
		endif()
	endif()

	foreach(type IN ITEMS RUNTIME LIBRARY ARCHIVE)
		if(ARGS_${type})
			set_target_properties(${target} PROPERTIES ${type}_OUTPUT_DIRECTORY ${ARGS_${type}})
			foreach(mode IN ITEMS DEBUG RELWITHDEBINFO RELEASE)
				set_target_properties(${target} PROPERTIES ${type}_OUTPUT_DIRECTORY_${mode} ${ARGS_${type}})
			endforeach()
		endif()
	endforeach()
endfunction()

## cmutils_target_set_classic_output_directories(target directory)
# Set the target runtime, library and archive output directory to classic folders build/bin and build/bin.
#   {value} [in] target:   Target to configure
macro(cmutils_target_set_classic_output_directories target)
	cmutils_target_set_output_directory(
		${target}
		RUNTIME "${CMAKE_CURRENT_BINARY_DIR}/build/bin"
		LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/build/lib"
		ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/build/lib"
	)
endmacro()

## cmutils_target_set_runtime_output_directory(target directory)
# Set the target runtime output directory.
#   {value} [in] target:      Target to configure
#   {value} [in] directory:   Output directory
macro(cmutils_target_set_runtime_output_directory target directory)
	cmutils_target_set_output_directory(${target} RUNTIME "${directory}")
endmacro()

## cmutils_target_set_library_output_directory(target directory)
# Set the target library output directory.
#   {value} [in] target:      Target to configure
#   {value} [in] directory:   Output directory
macro(cmutils_target_set_library_output_directory target directory)
	cmutils_target_set_output_directory(${target} LIBRARY "${directory}")
endmacro()

## cmutils_target_set_archive_output_directory(target directory)
# Set the target archive output directory.
#   {value} [in] target:      Target to configure
#   {value} [in] directory:   Output directory
macro(cmutils_target_set_archive_output_directory target directory)
	cmutils_target_set_output_directory(${target} ARCHIVE "${directory}")
endmacro()

## cmutils_target_configure_msvc_compile_options(target)
# Configure msvc compile oprions for the target like debug informations, optimisation...
#   {value} [in] target:   Target to configure
function(cmutils_target_configure_msvc_compile_options target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 1)
		message(FATAL_ERROR "Too many arguments")
	endif()

	foreach(lang IN ITEMS C CXX)
		# set Source and Executable character sets to UTF-8
		cmutils_target_add_compiler_flag(${target} ${lang} "/utf-8")

		# enable parallel compilation
		cmutils_target_add_compiler_flag(${target} ${lang} "/MP")

		# generates complete debugging information
		cmutils_target_add_compiler_flag(${target} ${lang} "/Zi" DEBUG RELWITHDEBINFO)
		cmutils_target_add_linker_flag(${target} ${lang} "/DEBUG:FULL" DEBUG RELWITHDEBINFO)

		# set optimization
		cmutils_target_add_compiler_flag(${target} ${lang} "/Od" DEBUG)
		cmutils_target_add_compiler_flag(${target} ${lang} "/O2" RELWITHDEBINFO)
		cmutils_target_add_compiler_flag(${target} ${lang} "/Ox" RELEASE)

		# enables automatic parallelization of loops
		cmutils_target_add_compiler_flag(${target} ${lang} "/Qpar" RELEASE)

		# enable runtime checks
		cmutils_target_add_compiler_flag(${target} ${lang} "/RTC1" DEBUG)

		# disable incremental compilations
		cmutils_target_add_linker_flag(${target} ${lang} "/INCREMENTAL:NO" RELEASE RELWITHDEBINFO)

		# remove unused symbols
		cmutils_target_add_linker_flag(${target} ${lang} "/OPT:REF" RELEASE RELWITHDEBINFO)
		cmutils_target_add_linker_flag(${target} ${lang} "/OPT:ICF" RELEASE RELWITHDEBINFO)

		# disable manifests
		cmutils_target_add_linker_flag(${target} ${lang} "/MANIFEST:NO" RELEASE RELWITHDEBINFO)

		# enable function-level linking
		#target_add_compiler_flag(${target} ${lang} "/Gy" RELEASE RELWITHDEBINFO)

		# sets the Checksum in the .exe header
		#target_add_linker_flag(${target} ${lang} "/RELEASE" RELEASE RELWITHDEBINFO)
	endforeach()
	message(STATUS "[cmutils] ${target}: configured msvc options")
endfunction()

## cmutils_target_configure_gcc_compile_options(target)
# Configure gcc compile oprions for the target like debug informations, optimisation...
#   {value} [in] target:   Target to configure
function(cmutils_target_configure_gcc_compile_options target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 1)
		message(FATAL_ERROR "Too many arguments")
	endif()

	foreach(lang IN ITEMS C CXX)
		# generates complete debugging information
		cmutils_target_add_compiler_flag(${target} ${lang} "-g3" DEBUG RELWITHDEBINFO)

		# set optimization
		cmutils_target_add_compiler_flag(${target} ${lang} "-O0" DEBUG)
		cmutils_target_add_compiler_flag(${target} ${lang} "-O2" RELWITHDEBINFO)
		cmutils_target_add_compiler_flag(${target} ${lang} "-O3" RELEASE)
	endforeach()

	# enable libstdc++ "debug" mode
	# warning: changes the size of some standard class templates
	# you cannot pass containers between translation units compiled
	# with and without libstdc++ "debug" mode
	#target_add_compile_definition(${target} _GLIBCXX_DEBUG DEBUG)
	#target_add_compile_definition(${target} _GLIBCXX_DEBUG_PEDANTIC DEBUG)
	#target_add_compile_definition(${target} _GLIBCXX_SANITIZE_VECTOR DEBUG)

	message(STATUS "[cmutils] ${target}: configured gcc options")
endfunction()

## cmutils_target_configure_clang_compile_options(target)
# Configure clang compile oprions for the target like debug informations, optimisation...
#   {value} [in] target:   Target to configure
function(cmutils_target_configure_clang_compile_options target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 1)
		message(FATAL_ERROR "Too many arguments")
	endif()

	foreach(lang IN ITEMS C CXX)
		# generates complete debugging information
		cmutils_target_add_compiler_flag(${target} ${lang} "-g3" DEBUG RELWITHDEBINFO)

		# set optimization
		cmutils_target_add_compiler_flag(${target} ${lang} "-O0" DEBUG)
		cmutils_target_add_compiler_flag(${target} ${lang} "-O2" RELWITHDEBINFO)
		cmutils_target_add_compiler_flag(${target} ${lang} "-O3" RELEASE)
	endforeach()

	# enable libstdc++ "debug" mode
	# warning: changes the size of some standard class templates
	# you cannot pass containers between translation units compiled
	# with and without libstdc++ "debug" mode
	#target_add_compile_definition(${target} _GLIBCXX_DEBUG DEBUG)
	#target_add_compile_definition(${target} _GLIBCXX_DEBUG_PEDANTIC DEBUG)
	#target_add_compile_definition(${target} _GLIBCXX_SANITIZE_VECTOR DEBUG)

	message(STATUS "[cmutils] ${target}: configured clang options")
endfunction()

## cmutils_target_configure_compile_options(target)
# Configure compile oprions for the target like debug informations, optimisation...
#   {value} [in] target:   Target to configure
function(cmutils_target_configure_compile_options target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 1)
		message(FATAL_ERROR "Too many arguments")
	endif()

	cmutils_define_compiler_variables()
	if(COMPILER_MSVC)
		cmutils_target_configure_msvc_compile_options(${target})
	elseif(COMPILER_GCC)
		cmutils_target_configure_gcc_compile_options(${target})
	elseif(COMPILER_CLANG)
		cmutils_target_configure_clang_compile_options(${target})
	else()
		message(WARNING "[cmutils] Unsupported compiler (${CMAKE_CXX_COMPILER_ID}), compile options not configured")
	endif()
endfunction()

## cmutils_target_set_runtime(target <STATIC|DYNAMIC>)
# Set target run-time: determine if the target should be linked statically
# or dynamically to the run-time library.
# MSVC infos: https://docs.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library
# GCC infos: https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html
#   {value}  [in] target:    Target to configure
#   {option} [in] STATIC:    If present, set static run-time
#   {option} [in] DYNAMIC:   If present, set dynamic run-time
function(cmutils_target_set_runtime target runtime)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 2)
		message(FATAL_ERROR "Too many arguments")
	endif()

	cmutils_define_compiler_variables()
	if(runtime STREQUAL "STATIC")
		if(COMPILER_MSVC)
			foreach(lang IN ITEMS C CXX)
				cmutils_target_add_linker_flag(${target} ${lang} "/MTd" DEBUG)
				cmutils_target_add_linker_flag(${target} ${lang} "/MT" RELWITHDEBINFO RELEASE)
			endforeach()
		elseif(COMPILER_GCC)
			cmutils_target_add_linker_flag(${target} CXX "-static-libstdc++")
			cmutils_target_add_linker_flag(${target} CXX "-static-libgcc")
			cmutils_target_add_linker_flag(${target} C "-static-libgcc")
		elseif(COMPILER_CLANG)
			cmutils_target_add_linker_flag(${target} CXX "-static-libstdc++")
			cmutils_target_add_linker_flag(${target} CXX "-static-libgcc")
			cmutils_target_add_linker_flag(${target} C "-static-libgcc")
		else()
			message(WARNING "[cmutils] Unsupported compiler (${CMAKE_CXX_COMPILER_ID}), run-time library not forced to static link")
			return()
		endif()
		message(STATUS "[cmutils] ${target}: set static run-time")
	elseif(runtime STREQUAL "DYNAMIC")
		if(COMPILER_MSVC)
			foreach(lang IN ITEMS C CXX)
				cmutils_target_add_linker_flag(${target} ${lang} "/MDd" DEBUG)
				cmutils_target_add_linker_flag(${target} ${lang} "/MD" RELWITHDEBINFO RELEASE)
			endforeach()
		elseif(COMPILER_GCC)
			# dynamic by default
		elseif(COMPILER_CLANG)
			# dynamic by default
		else()
			message(WARNING "[cmutils] Unsupported compiler (${CMAKE_CXX_COMPILER_ID}), run-time library not forced to dynamic link")
			return()
		endif()
		message(STATUS "[cmutils] ${target}: set dynamic run-time")
	else()
		message(FATAL_ERROR "Invalid argument: ${runtime}")
	endif()
endfunction()

## cmutils_target_enable_sanitizers(target sanitizers... [configs...])
# Enable the specified sanitizers in the specified build modes for the specified target on compilers
# that support the sanitizers.
# Incompatibilities: ThreadSanitizer cannot be combined with AddressSanitizer and LeakSanitizer
# Value -> sanitizer correspondance:
# - ADDRESS:   AddressSanitizer
# - THREAD:    ThreadSanitizer
# - LEAK:      LeakSanitizer
# - UNDEFINED: UndefinedBehaviorSanitizer
# - MEMORY:    MemorySanitizer
#   {value} [in] sanitizers:   Sanitizers to enable (ADDRESS|THREAD|LEAK|UNDEFINED|MEMORY)
#   {value} [in] configs:      Configs to enable sanitizers (DEBUG|RELEASE|RELWITHDEBINFO)
function(cmutils_target_enable_sanitizers target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	cmake_parse_arguments(ARGS "ADDRESS;THREAD;LEAK;UNDEFINED;MEMORY;DEBUG;RELWITHDEBINFO;RELEASE" "" "" ${ARGN})
	if(ARGS_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Invalid arguments: ${ARGS_UNPARSED_ARGUMENTS}")
	endif()

	# Incompatibilities documented at:
	# https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#Instrumentation-Options
	if(ARGS_THREAD AND ARGS_ADDRESS)
		message(FATAL_ERROR "ThreadSanitizer and AddressSanitizer cannot be combined")
	endif()
	if(ARGS_THREAD AND ARGS_LEAK)
		message(FATAL_ERROR "ThreadSanitizer and LeakSanitizer cannot be combined")
	endif()

	cmutils_define_compiler_variables()
	if(NOT (COMPILER_CLANG OR COMPILER_GCC))
		# Sanitizers supported only by gcc and clang
		return()
	endif()

	set(configs)
	foreach(config IN ITEMS DEBUG RELWITHDEBINFO RELEASE)
		if(ARGS_${config})
			list(APPEND configs ${config})
		endif()
	endforeach()

	set(flags)
	if(ARGS_ADDRESS)
		message(STATUS "[cmutils] ${target}: enable (if available) AddressSanitizer")
		list(APPEND flags "-fsanitize=address")
		list(APPEND flags "-fno-omit-frame-pointer")
		list(APPEND flags "-fno-optimize-sibling-calls")
	endif()
	if(ARGS_THREAD)
		message(STATUS "[cmutils] ${target}: enable (if available) ThreadSanitizer")
		list(APPEND flags "-fsanitize=thread")
	endif()
	if(ARGS_LEAK)
		message(STATUS "[cmutils] ${target}: enable (if available) LeakSanitizer")
		list(APPEND flags "-fsanitize=leak")
	endif()
	if(ARGS_UNDEFINED AND COMPILER_GCC) # need to use clang as linker with clang as compiler but cmake uses ld
		message(STATUS "[cmutils] ${target}: enable (if available) UndefinedBehaviorSanitizer")
		list(APPEND flags "-fsanitize=undefined")
	endif()
	if(ARGS_MEMORY AND COMPILER_CLANG) # only available in clang
		message(STATUS "[cmutils] ${target}: enable (if available) MemorySanitizer")
		list(APPEND flags "-fsanitize=memory")
	endif()

	foreach(flag ${flags})
		foreach(lang IN ITEMS C CXX)
			if(configs)
				cmutils_target_add_compiler_flag(${target} ${lang} "${flag}" ${configs})
				cmutils_target_add_linker_flag(${target} ${lang} "${flag}" ${configs})
			else()
				cmutils_target_add_compiler_flag(${target} ${lang} "${flag}")
				cmutils_target_add_linker_flag(${target} ${lang} "${flag}")
			endif()
		endforeach()
	endforeach()
endfunction()

## cmutils_target_enable_warnings(target [json_flags_files...])
# Enable warnings for the specified target. Flags are loaded from the specified JSON files or,
# if no files are specified, loaded from default file default-flags.json.
# Enable flags only if they are supported by the compiler.
# See default-flags.json for a JSON flags file example.
#   {value} [in] target:             Target to configure
#   {value} [in] json_flags_files:   JSON files paths
function(cmutils_target_enable_warnings target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 1)
		foreach(file_path ${ARGN})
			if(NOT EXISTS "${file_path}")
				message(FATAL_ERROR "Invalid flags file (doesn't exist or can't be read): ${file_path}")
			endif()
		endforeach()
		cmutils_load_flags(${target} ${ARGN})
	else()
		cmutils_load_default_flags(${target})
	endif()

	cmutils_define_compiler_variables()
	if(COMPILER_MSVC)
		set(compiler MSVC)
	elseif(COMPILER_GCC)
		set(compiler GCC)
	elseif(COMPILER_CLANG)
		set(compiler CLANG)
	else()
		message(WARNING "[cmutils] Unsupported compiler (${CMAKE_CXX_COMPILER_ID}), warnings not enabled for ${target}")
		return()
	endif()

	foreach(flag ${${target}_${compiler}_COMMON_FLAGS})
		foreach(lang IN ITEMS C CXX)
			cmutils_target_add_compiler_flag(${target} ${lang} ${flag})
		endforeach()
	endforeach()
	foreach(lang IN ITEMS C CXX)
		foreach(flag ${${target}_${compiler}_${lang}_FLAGS})
			cmutils_target_add_compiler_flag(${target} ${lang} ${flag})
		endforeach()
	endforeach()
	message(STATUS "[cmutils] ${target}: enabled warnings")
endfunction()

## cmutils_target_disable_warnings(target)
# Disable warnings for the specified target.
#   {value} [in] target:   Target to configure
function(cmutils_target_disable_warnings target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 1)
		message(FATAL_ERROR "Too many arguments")
	endif()

	cmutils_define_compiler_variables()
	if(NOT (COMPILER_MSVC OR COMPILER_GCC OR COMPILER_CLANG))
		message(WARNING "[cmutils] Unsupported compiler (${CMAKE_CXX_COMPILER_ID}), warnings not disabled for ${target}")
		return()
	endif()

	foreach(lang IN ITEMS C CXX)
		if(COMPILER_MSVC)
			cmutils_target_add_compiler_flag(${target} ${lang} "/W0")
		elseif(COMPILER_GCC)
			cmutils_target_add_compiler_flag(${target} ${lang} "--no-warnings")
		elseif(COMPILER_CLANG)
			cmutils_target_add_compiler_flag(${target} ${lang} "-Wno-everything")
		endif()
	endforeach()
	message(STATUS "[cmutils] ${target}: disabled warnings")
endfunction()

## cmutils_target_set_ide_folder(target folder)
# Set target folder for IDEs.
#   {value} [in] target:   Target to configure
function(cmutils_target_set_ide_folder target folder)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 2)
		message(FATAL_ERROR "Too many arguments")
	endif()

	set_target_properties(${target} PROPERTIES FOLDER ${folder})
endfunction()

## cmutils_target_source_group(target root)
# Group sources of target relatively to the specified root to keep structure of source groups
# analogically to the actual files and directories structure in the project.
#   {value} [in] target:   Target to configure
#   {value} [in] root:     Root to group files relatives to
function(cmutils_target_source_group target root)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "Invalid argument: ${target} is not a target")
	endif()
	if(${ARGC} GREATER 2)
		message(FATAL_ERROR "Too many arguments")
	endif()

	get_property(target_sources TARGET ${target} PROPERTY SOURCES)
	source_group(TREE ${root} FILES ${target_sources})
endfunction()
