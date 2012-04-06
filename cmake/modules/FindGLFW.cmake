
SET(GLFW_ROOT_PATH F:/dok/kode/libraries/glfw-2.7.2/ )


FIND_PATH( GLFW_INCLUDE_DIR NAMES GL/glfw.h 
		PATHS ${GLFW_ROOT_PATH}/include
	)

FIND_LIBRARY( GLFW_LIB NAMES glfw
		PATHS ${GLFW_ROOT_PATH}/bin/lib-msvc100/debug
	) 


