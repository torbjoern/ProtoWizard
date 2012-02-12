include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (fractal2d main.cpp ) 

target_link_libraries (fractal2d PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} ${SOIL_LIB} )
