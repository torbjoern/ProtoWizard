include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (ballbearing main.cpp ) 

target_link_libraries (ballbearing PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} )
