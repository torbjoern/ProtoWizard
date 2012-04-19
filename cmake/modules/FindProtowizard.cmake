
SET(PROTOWIZARD_ROOT_PATH ${CMAKE_MODULE_PATH}/../../protowizard )

FIND_PATH( PROTOWIZARD_INCLUDE_DIR NAMES protographics.h
		PATHS ${PROTOWIZARD_ROOT_PATH}
	)


# smarter to use /build/bin/lib ?
# or bin/lib?
set(PROTOWIZARD_DEBUG ${BULLET_DEBUG_LIBS} 
	${PROTOWIZARD_ROOT_PATH}/lib/protowizard_debug.lib
	)
set(PROTOWIZARD_RELEASE ${BULLET_RELEASE_LIBS} 
	${PROTOWIZARD_ROOT_PATH}/lib/protowizard.lib
	)
