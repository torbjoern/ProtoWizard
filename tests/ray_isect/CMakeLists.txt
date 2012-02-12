include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (ray_isect main.cpp ) 

target_link_libraries (ray_isect PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} )
