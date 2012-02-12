include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (blending main.cpp ) 

target_link_libraries (blending PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} )
