include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (voxel_test main.cpp stb_image.c ) 

target_link_libraries (voxel_test PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} ${SOIL_LIB} )
