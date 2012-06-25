add_executable (music_viz main.cpp ) 
target_link_libraries (music_viz bass_wrapper ${proto_lib_list} ${BASS_LIB} )
