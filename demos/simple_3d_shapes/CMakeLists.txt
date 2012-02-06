include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

include_directories (${KIXXOR_OBJ_LOADER_SOURCE_DIR}/KIXXOR_OBJ_LOADER ) 
link_directories (${KIXXOR_OBJ_LOADER_BINARY_DIR}/KIXXOR_OBJ_LOADER ) 

add_executable (simple_3d_shapes main.cpp ) 

target_link_libraries (simple_3d_shapes PROTOWIZARD kixxor_obj_loader ${OPENGL_LIBRARIES} ${GLEW_LIB}  ${GLFW_LIB} ${SOIL_LIB} )
