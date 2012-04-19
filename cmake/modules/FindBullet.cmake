SET(BULLET_ROOT_PATH F:/code_lab/bullet-2.79 )

FIND_PATH( BULLET_INCLUDE_DIR NAMES btBulletDynamicsCommon.h
  PATHS ${BULLET_ROOT_PATH}/src
 )

set(BULLET_DEBUG_LIBS ${BULLET_DEBUG_LIBS} 
 ${BULLET_ROOT_PATH}/lib/LinearMath_debug.lib
 ${BULLET_ROOT_PATH}/lib/BulletDynamics_debug.lib
 ${BULLET_ROOT_PATH}/lib/BulletCollision_debug.lib
 )
set(BULLET_RELEASE_LIBS ${BULLET_RELEASE_LIBS} 
 ${BULLET_ROOT_PATH}/lib/LinearMath.lib
 ${BULLET_ROOT_PATH}/lib/BulletDynamics.lib
 ${BULLET_ROOT_PATH}/lib/BulletCollision.lib
 )