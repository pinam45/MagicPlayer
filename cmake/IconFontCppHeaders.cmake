message(STATUS "Configuring IconFontCppHeaders")

get_filename_component(ICON_FONT_CPP_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/IconFontCppHeaders ABSOLUTE)

# Variables
get_filename_component(ICON_FONT_CPP_HEADERS_INCLUDE_DIR  ${ICON_FONT_CPP_HEADERS_DIR}  ABSOLUTE)
set(ICON_FONT_CPP_HEADERS_LIBRARY "")

# Message
message("> include: ${ICON_FONT_CPP_HEADERS_INCLUDE_DIR}")
message(STATUS "Configuring IconFontCppHeaders - Done")
