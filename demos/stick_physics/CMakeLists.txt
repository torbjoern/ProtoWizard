include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (stick_physics main.cpp ) 

target_link_libraries (stick_physics PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} ${SOIL_LIB} )

