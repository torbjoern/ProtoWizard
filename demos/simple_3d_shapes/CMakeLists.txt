include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (simple_3d_shapes main.cpp ) 

target_link_libraries (simple_3d_shapes PROTOWIZARD bass_wrapper ${OPENGL_LIBRARIES} ${GLFW_LIB} ${SOIL_LIB} ${BASS_LIB} )
