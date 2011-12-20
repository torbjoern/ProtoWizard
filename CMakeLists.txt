cmake_minimum_required (VERSION 2.6)
project (ProtoWizard)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

find_package(OPENGL REQUIRED)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_PATH})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

find_package(ANGELSCRIPT REQUIRED)
include_directories(${ANGELSCRIPT_INCLUDE_DIR})
include_directories(${ANGELSCRIPT_ADDON_INCLUDE_DIR})

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
	platform.h
	color_utils.h
	
	depends/vsml.h
	depends/noise/perlin.h
)

set(SRCS
	main.cpp
	protographics.cpp
	shader.cpp
	platform.cpp
	color_utils.cpp
	
	depends/vsml.cpp
	depends/noise/perlin.cpp
)



# add the executable
add_executable ( ProtoWizard main.cpp ${ANGELSCRIPT_INCLUDE} ${GLEW_INCLUDE_DIR} ${HDRS} ${SRCS} )

  
  
IF(CMAKE_CXX_FLAGS_DEBUG)
	message("DEBUG version" ) 
	target_link_libraries (ProtoWizard ${ANGELSCRIPT_SCRIPTARRAY_LIB} ${ANGELSCRIPT_SCRIPTMATH_LIB} ${ANGELSCRIPT_DEBUG_LIB} ${OPENGL_LIBRARIES} ${GLEW_LIB}  ${GLFW_LIB}  )
ELSEIF(CMAKE_CXX_FLAGS_RELEASE)
	#message("RELEASE version" ) 
	target_link_libraries (ProtoWizard ${ANGELSCRIPT_SCRIPTMATH_LIB} ${ANGELSCRIPT_RELEASE_LIB} ${OPENGL_LIBRARIES} ${GLEW_LIB}  ${GLFW_LIB}  )
ENDIF(CMAKE_CXX_FLAGS_DEBUG)

