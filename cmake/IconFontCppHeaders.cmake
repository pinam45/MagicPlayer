message(STATUS "Configuring IconFontCppHeaders")

get_filename_component(ICON_FONT_CPP_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/IconFontCppHeaders ABSOLUTE)

# Declare IconFontCppHeaders
add_library(IconFontCppHeaders INTERFACE)

# Add includes
target_include_directories(
	IconFontCppHeaders SYSTEM INTERFACE
	"${ICON_FONT_CPP_HEADERS_DIR}"
)

# Headers target for ide
cmutils_interface_target_generate_headers_target(IconFontCppHeaders IconFontCppHeaders_headers_for_ide)
cmutils_target_source_group(IconFontCppHeaders_headers_for_ide "${ICON_FONT_CPP_HEADERS_DIR}")
cmutils_target_set_ide_folder(IconFontCppHeaders_headers_for_ide "extlibs/IconFontCppHeaders")

message(STATUS "Configuring IconFontCppHeaders - Done")
