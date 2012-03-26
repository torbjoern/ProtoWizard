include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (music_visualizer main.cpp ) 

target_link_libraries (music_visualizer PROTOWIZARD bass_wrapper ${OPENGL_LIBRARIES} ${GLFW_LIB} ${BASS_LIB} )
