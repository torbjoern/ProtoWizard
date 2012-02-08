cmake_minimum_required (VERSION 2.6)

include_directories (${KIXXOR_OBJ_LOADER_SOURCE_DIR}/KIXXOR_OBJ_LOADER ) 
link_directories (${KIXXOR_OBJ_LOADER_BINARY_DIR}/KIXXOR_OBJ_LOADER ) 

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
	shapes/mesh.h

	texture_manager.h
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
	shapes/mesh.cpp

	texture_manager.cpp
)

set(SHADERS
	shader2d.frag
	shader2d.vert

	line2d_shader.frag
	line2d_shader.vert

	phong.frag
	phong.vert

	passthru.vert
	normals.gs
	normals.frag
)


add_library( ProtoWizard ${HDRS} ${SRCS} )
