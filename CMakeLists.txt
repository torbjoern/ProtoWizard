cmake_minimum_required (VERSION 2.8) 
project (PROTO) 

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib")

find_package(OPENGL REQUIRED)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

find_package(BASS REQUIRED)
include_directories(${BASS_INCLUDE_DIR})

#include_directories( ${CMAKE_SOURCE_DIR}/depends/include/ )
include_directories (protowizard) 
link_directories (${CMAKE_SOURCE_DIR}/lib) 

set( proto_lib_list
	protowizard
	obj_loader
	${OPENGL_LIBRARIES} 
	debug ${GLFW_STATIC_DEBUG_LIBRARY} optimized ${GLFW_STATIC_RELEASE_LIBRARY}
)


add_subdirectory (protowizard) 
add_subdirectory (depends/bass_wrapper)
add_subdirectory (depends/obj_loader)

add_subdirectory (demos/stress_test)
add_subdirectory (demos/stick_physics)
add_subdirectory (demos/simple_3d_shapes)
add_subdirectory (demos/music_viz)
add_subdirectory (demos/fractal2d)
add_subdirectory (demos/blending)
add_subdirectory (demos/2dphysics)



add_subdirectory (tests/obj_model)
add_subdirectory (tests/ray_isect)
add_subdirectory (tests/dda_3d)
add_subdirectory (tests/random)

add_subdirectory (games/ballbearing)
add_subdirectory (games/bilspel)
add_subdirectory (games/tronlike)