message(STATUS "Configuring SFML")

# Compile with project option
option(MAGICPLAYER_COMPILE_SFML_WITH_PROJECT "Compile SFML with project, don't search system installation" OFF)

# Config
set(SFML_MINIMUM_SYSTEM_VERSION 2.5)

set(SFML_USE_EMBEDED ${MAGICPLAYER_COMPILE_SFML_WITH_PROJECT})
if(NOT SFML_USE_EMBEDED)
	cmutils_define_os_variables()
	if(OS_WINDOWS)
		set(SFML_USE_EMBEDED ON)
		message(STATUS "OS is Windows, compile SFML with project")
	else()
		find_package(SFML ${SFML_MINIMUM_SYSTEM_VERSION} COMPONENTS system window graphics audio CONFIG)
		if(SFML_FOUND)
			message(WARNING
				"If the program crashes when using the clipboard,"
				" it is an SFML bug not yet fixed on your system but already fixed on Github (see https://github.com/SFML/SFML/pull/1437),"
				" use CMake with -DMAGICPLAYER_COMPILE_SFML_WITH_PROJECT=ON to compile and use the fixed version with the project"
				)
		else()
			set(SFML_USE_EMBEDED ON)
			message(STATUS "SFML system installation not found, compile SFML with project")
		endif()
	endif()
endif()

if(SFML_USE_EMBEDED)
	get_filename_component(SFML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SFML ABSOLUTE)

	# Submodule check
	cmutils_directory_is_empty(is_empty "${SFML_DIR}")
	if(is_empty)
		message(FATAL_ERROR "SFML dependency is missing, maybe you didn't pull the git submodules")
	endif()

	# Subproject
	add_subdirectory(${SFML_DIR})

	# Configure targets
	foreach(sfml_target IN ITEMS sfml-system sfml-network sfml-window sfml-graphics sfml-audio sfml-main)
		if(TARGET ${sfml_target})
			# Configure SFML folder in IDE
			cmutils_target_set_ide_folder(${sfml_target} "extlibs/SFML")

			# Disable warnings on headers
			get_target_property(target_include_directories ${sfml_target} INTERFACE_INCLUDE_DIRECTORIES)
			set_target_properties(${sfml_target} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
			target_include_directories(${sfml_target} SYSTEM INTERFACE ${target_include_directories})

			# Setup output, put MagicPlayer executable and required SFML dll in the same folder
			cmutils_target_set_output_directory(${sfml_target} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
		endif()
	endforeach()

	# Configure OpenAL
	cmutils_define_os_variables()
	if(OS_WINDOWS)
		cmutils_define_arch_variables()
		set(ARCH_FOLDER "x86")
		if(ARCH_64BITS)
			set(ARCH_FOLDER "x64")
		endif()
		configure_file(${SFML_DIR}/extlibs/bin/${ARCH_FOLDER}/openal32.dll "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" COPYONLY)
	endif()
endif()

message(STATUS "Configuring SFML - Done")
