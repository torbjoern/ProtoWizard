include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (stress_test main.cpp ) 

target_link_libraries (stress_test PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} ${SOIL_LIB} )
