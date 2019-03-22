message(STATUS "Configuring OpenGL")

# Find OpenGL
set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
	message(FATAL_ERROR "OpenGL not found")
	return()
endif()

if(NOT TARGET OpenGL::GL)
	message(FATAL_ERROR "OpenGL::GL target is missing")
endif()

# Message
message(STATUS "Configuring OpenGL - Done")
