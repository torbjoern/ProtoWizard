cmake_minimum_required (VERSION 2.6)


set(CMAKE_MODULE_PATH F:/gitrepos/cmake/modules/)

find_package(OPENGL REQUIRED)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

set(HDRS
	camera.h
	color_utils.h
	common.h
	platform.h
	protographics.h
	shader.h
	vertex_types.h
	
	depends/vsml.h
	depends/noise/perlin.h
	
	shapes/sphere.h
	shapes/shapes.h
	shapes/plane.h
	shapes/line.h
	shapes/cylinder.h
	shapes/cube.h
	shapes/circle.h
	
)

set(SRCS
	camera.cpp
	color_utils.cpp
	platform.cpp
	protographics.cpp
	shader.cpp
	
	depends/vsml.cpp
	depends/noise/perlin.cpp
	
	shapes/shapes.cpp
)


add_library( ProtoWizard ${HDRS} ${SRCS} )

set_target_properties(ProtoWizard
      PROPERTIES
      target_link_libraries (ProtoWizard ${OPENGL_LIBRARIES} ${GLEW_LIB} ${GLFW_LIB} )
  )

