cmake_minimum_required (VERSION 2.8)

######################## set the path of dependencies so they can be linked into Protowizard.lib easily

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
  ${noise_HDRS}
  ${gl3w_HDRS}
  ${soil_HDRS}
)

set(Dependency_SRCS
  ${noise_SRCS}
  ${gl3w_SRCS}
  ${soil_SRCS}
)

######################## OK, now for ProtoWizard headers and sources:

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set(math_HDRS
  proto/math/math_common.h
  proto/math/math_line.h
  proto/math/math_ray.h
  proto/math/random.h
)
set(math_SRCS
  src/random.cpp
)
SOURCE_GROUP("Header Files\\math" FILES ${math_HDRS})
SOURCE_GROUP("Source Files\\math" FILES ${math_SRCS})

set(shape_HDRS
	proto/geo_cube.h
	proto/geo_cylinder.h
	proto/geo_plane.h
	proto/geo_sphere.h

	proto/shapes/line.h
	proto/shapes/circle.h
	proto/shapes/mesh.h
)
set(shape_SRCS
	src/geo_cube.cpp
	src/geo_cylinder.cpp
	src/geo_plane.cpp
	src/geo_sphere.cpp
)

SOURCE_GROUP("Header Files\\shapes" FILES ${shape_HDRS})
SOURCE_GROUP("Source Files\\shapes" FILES ${shape_SRCS})

set(HDRS
	${math_HDRS}
	${shape_HDRS}
	proto/camera.h
	proto/color_utils.h
	proto/opengl_stuff.h
	proto/platform.h
	proto/protographics.h
	proto/shader.h
	proto/texture_manager.h
	proto/mesh_manager.h
	proto/vertex_types.h
	proto/fileio/text_file.h
	proto/path.h
	proto/string_utils.h
	proto/gl_attrib.h
	proto/gl_vao.h
	proto/gl_vbo.h
	proto/gl_ibo.h
)

set(SRCS
	${math_SRCS}
	${shape_SRCS}
	src/camera.cpp
	src/color_utils.cpp
	src/platform.cpp
	src/protographics.cpp
	src/shader.cpp
	src/shapes.cpp
	src/mesh.cpp
	src/texture_manager.cpp
	src/mesh_manager.cpp
	src/text_file.cpp
	src/path.cpp
	src/gl_vao.cpp
	src/gl_vbo.cpp
	src/gl_ibo.cpp
)

add_library( protowizard STATIC ${Dependency_HDRS} ${HDRS} ${Dependency_SRCS} ${SRCS} )
