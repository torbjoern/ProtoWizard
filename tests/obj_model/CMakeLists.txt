include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (obj_model main.cpp ) 

target_link_libraries (obj_model PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} ${SOIL_LIB} )
