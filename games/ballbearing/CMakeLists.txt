find_package(Bullet REQUIRED)

include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
include_directories (${BULLET_ROOT_PATH}/src) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (ballbearing main.cpp ) 

target_link_libraries(ballbearing
	PROTOWIZARD
	${OPENGL_LIBRARIES}
	debug ${GLFW_STATIC_DEBUG_LIBRARY} optimized ${GLFW_STATIC_RELEASE_LIBRARY}
	debug ${BULLET_DEBUG_LIBS} optimized ${BULLET_RELEASE_LIBS}
)