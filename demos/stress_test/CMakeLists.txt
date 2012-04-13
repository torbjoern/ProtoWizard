include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (stress_test main.cpp ) 

target_link_libraries (stress_test 
	PROTOWIZARD 
	${OPENGL_LIBRARIES} 
	debug ${GLFW_STATIC_DEBUG_LIBRARY} optimized ${GLFW_STATIC_RELEASE_LIBRARY}
)
