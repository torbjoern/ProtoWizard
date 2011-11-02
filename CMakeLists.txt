cmake_minimum_required (VERSION 2.6)
project (ProtoWizard)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_PATH})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

set(HDRS
	camera.h
	common.h
	protographics.h
	shader.h
	vertex_types.h
	shapes/sphere.h
	shapes/cylinder.h
	shapes/circle.h
	shapes/line.h
	shapes/cube.h
	shapes/plane.h
	shapes/shapes.h
	
	depends/vsml.h
	depends/noise/perlin.h
)

set(SRCS
	main.cpp
	protographics.cpp
	shader.cpp
	depends/vsml.cpp
	depends/noise/perlin.cpp
)



# add the executable
add_executable ( ProtoWizard main.cpp ${GLEW_INCLUDE_DIR} ${HDRS} ${SRCS} )
target_link_libraries (ProtoWizard  ${GLEW_LIB}  ${GLFW_LIB} )