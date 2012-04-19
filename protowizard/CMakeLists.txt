cmake_minimum_required (VERSION 2.6)

######################## set the path of dependencies so they can be linked into Protowizard.lib easily

set(kixxor_obj_loader_HDRS
	../depends/obj_loader/list.h
	../depends/obj_loader/obj_parser.h
	../depends/obj_loader/objLoader.h
	../depends/obj_loader/string_extra.h
)
set(kixxor_obj_loader_SRCS
	../depends/obj_loader/list.cpp
	../depends/obj_loader/obj_parser.cpp
	../depends/obj_loader/objLoader.cpp
	../depends/obj_loader/string_extra.cpp
)
SOURCE_GROUP("Header Files\\Dependency\\kixxor_obj_loader" FILES ${kixxor_obj_loader_HDRS})
SOURCE_GROUP("Source Files\\Dependency\\kixxor_obj_loader" FILES ${kixxor_obj_loader_SRCS})


set(noise_HDRS ../depends/noise/perlin.h )
set(noise_SRCS ../depends/noise/perlin.cpp )
SOURCE_GROUP("Header Files\\Dependency\\noise" FILES ${noise_HDRS})
SOURCE_GROUP("Source Files\\Dependency\\noise" FILES ${noise_SRCS})

set(gl3w_HDRS 
  ../depends/gl3w/gl3.h
  ../depends/gl3w/gl3w.h 
)
set(gl3w_SRCS ../depends/gl3w/gl3w.c )
SOURCE_GROUP("Header Files\\Dependency\\gl3w" FILES ${gl3w_HDRS})
SOURCE_GROUP("Source Files\\Dependency\\gl3w" FILES ${gl3w_SRCS})

set(soil_HDRS
	../depends/soil/image_DXT.h
	../depends/soil/image_helper.h
	../depends/soil/SOIL.h
	../depends/soil/stbi_DDS_aug.h
	../depends/soil/stb_image_aug.h
	../depends/soil/stbi_DDS_aug_c.h
)
set(soil_SRCS
	../depends/soil/image_DXT.c
	../depends/soil/image_helper.c
	../depends/soil/SOIL.c
	../depends/soil/stb_image_aug.c
)
SOURCE_GROUP("Header Files\\Dependency\\soil" FILES ${soil_HDRS})
SOURCE_GROUP("Source Files\\Dependency\\soil" FILES ${soil_SRCS})

######################## finally, put all dependency .h and sources into a couple of handy variables:
set(Dependency_HDRS
  ${kixxor_obj_loader_HDRS}
  ${noise_HDRS}
  ${gl3w_HDRS}
  ${soil_HDRS}
)

set(Dependency_SRCS
  ${kixxor_obj_loader_SRCS}
  ${noise_SRCS}
  ${gl3w_SRCS}
  ${soil_SRCS}
)

######################## OK, now for ProtoWizard headers and sources:

set(math_HDRS
  math/math_common.h
  math/math_line.h
  math/math_ray.h
)

set(shape_HDRS
	shapes/sphere.h
	shapes/shapes.h
	shapes/plane.h
	shapes/line.h
	shapes/cylinder.h
	shapes/cube.h
	shapes/circle.h
	shapes/mesh.h
)
SOURCE_GROUP("Header Files\\math" FILES ${math_HDRS})
SOURCE_GROUP("Header Files\\shapes" FILES ${shape_HDRS})

set(HDRS
	${math_HDRS}
	${shape_HDRS}
	camera.h
	color_utils.h
	common.h
	opengl_stuff.h
	platform.h
	protographics.h
	shader.h
	texture_manager.h
	mesh_manager.h
	vertex_types.h
	fileio/text_file.h
	path.h
)

set(SRCS
	camera.cpp
	color_utils.cpp
	platform.cpp
	protographics.cpp
	shader.cpp
	shapes/shapes.cpp
	shapes/mesh.cpp
	texture_manager.cpp
	mesh_manager.cpp
	fileio/text_file.cpp
	path.cpp
)

set(shader_SRCS
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
# has no effect SOURCE_GROUP("Resource Files\\shaders" FILES ${shader_SRCS})

add_library( ProtoWizard ${Dependency_HDRS} ${HDRS} ${Dependency_SRCS} ${SRCS} )
