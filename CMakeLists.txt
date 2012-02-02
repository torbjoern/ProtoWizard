cmake_minimum_required (VERSION 2.6)

set(HDRS
	camera.h
	color_utils.h
	common.h
	gfx_wrapper.h
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
	gfx_wrapper.cpp
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

