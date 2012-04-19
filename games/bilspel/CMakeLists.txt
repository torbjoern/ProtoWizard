
#find_package(Protowizard REQUIRED)
#include_directories (${PROTOWIZARD_INCLUDE_DIR}) # can a find_package also find my includes?

include_directories (${PROTOWIZARD_SOURCE_DIR}/PROTOWIZARD) 
link_directories (${PROTOWIZARD_BINARY_DIR}/PROTOWIZARD) 

add_executable (bilspel main.cpp ) 

target_link_libraries(bilspel
	PROTOWIZARD
	${OPENGL_LIBRARIES}
	debug ${GLFW_STATIC_DEBUG_LIBRARY} optimized ${GLFW_STATIC_RELEASE_LIBRARY}
)