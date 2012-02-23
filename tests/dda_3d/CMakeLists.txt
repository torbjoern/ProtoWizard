include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (dda3d main.cpp ) 

target_link_libraries (dda3d PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} )
