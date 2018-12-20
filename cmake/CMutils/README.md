# CMake utilities

Some CMake utility functions to configure compile options, load and enable warnings from JSON files, manipulate lists, enable sanitizers, configure ccache and clang-format...

Functions are gathered in different modules (files actually), each modules can be included independently, for example:
```cmake
include(CMUtils/cmutils-clang-format.cmake)
```
will include *clang-format* module and the modules it depends on. But you can also just include `cmutils.cmake` which will include all modules.

Each function has a dedicated documentation before its definition.

## Use example

For this example we will consider a project with the following files/folders structure:
```
-Project root
 |-CMutils
 | |-cmutils.cmake
 | |-cmutils-ccache.cmake
 | |-...
 |-src
 |-include
 |-externals
 | |-spdlog
 | | |-include
 | | | ...
 | |-...
 |-resources
 |-CMakeLists.txt
 |-warnings.json
 |-...
```
with `Project root/CMutils` containing a clone of the CMutils repos. And `warnings.json` containing:
```json
{
	"msvc": {
		"common": [
			"/W4"
		],
		"c++": [
			"/w44263",
			"/w44265"
		]
	},
	"gcc": {
		"common": [
			"-pedantic",
			"-Wall",
			"-Wextra"
		],
		"c++": [
			"-Wzero-as-null-pointer-constant"
		],
		"c": [
			"-Wdeclaration-after-statement",
			"-Wbad-function-cast"
		]
	},
	"clang": {
		"common": [
			"-Wall",
			"-Wbad-function-cast",
			"-Wfatal-errors"
		]
	}
}
```

The `CMakeLists.txt` can contain something like this:
```cmake
# CMake 3.10 for CXX_STANDARD 17 property on Visual Studio
cmake_minimum_required(VERSION 3.10)

# Project declaration
project(
	Example
	VERSION 1.0.0
	DESCRIPTION "CMutils example project"
	LANGUAGES CXX
)

# include CMake utilities
include(CMutils/cmutils.cmake)

# Global config
cmutils_disable_in_sources_build()
cmutils_enable_ide_folders()
cmutils_set_classic_output_folders()

# Declare Example
add_executable(Example)

# Add sources
cmutils_target_sources_folders(
	Example PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/include"
)

# Add includes
target_include_directories(
	Example PRIVATE
	"${CMAKE_SOURCE_DIR}/include"
)

# Add external includes
target_include_directories(
	Example SYSTEM PRIVATE
	"${CMAKE_SOURCE_DIR}/external/spdlog/include"
)

# Link dependencies
find_package(Threads REQUIRED)
target_link_libraries(Example PRIVATE Threads::Threads)

# Configure compile options
cmutils_target_configure_compile_options(Example)

# Enable warnings from warnings.json
cmutils_target_enable_warnings(Example "${CMAKE_SOURCE_DIR}/warnings.json")

# Build in C++17
cmutils_target_set_standard(Example CXX 17)

# Set static runtime
cmutils_target_set_runtime(Example STATIC)

# Enable sanitizers in debug
cmutils_target_enable_sanitizers(
	Example
	ADDRESS LEAK UNDEFINED MEMORY
	DEBUG
)

# Set target IDE folder
cmutils_target_set_ide_folder(Example "Example-projects")

# Group sources for IDEs
cmutils_target_source_group(Example "${CMAKE_SOURCE_DIR}")

# Use ccache
cmutils_target_use_ccache(Example)

# Generate format target
cmutils_target_generate_clang_format(Example)
```

For a *real world* usage example, you can check the [MagicPlayer project](https://github.com/pinam45/MagicPlayer).