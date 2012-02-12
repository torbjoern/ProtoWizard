include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (game_tron main.cpp ) 

target_link_libraries (game_tron PROTOWIZARD ${OPENGL_LIBRARIES} ${GLFW_LIB} )
