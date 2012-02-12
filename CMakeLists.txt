cmake_minimum_required (VERSION 2.6) 
project (PROTOWIZARD) 

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

set(CMAKE_MODULE_PATH F:/gitrepos/cmake/modules)

find_package(OPENGL REQUIRED)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

find_package(BASS REQUIRED)
include_directories(${BASS_INCLUDE_DIR})

add_subdirectory (protowizard) 
add_subdirectory (depends/bass_wrapper)

add_subdirectory (demos/simple_3d_shapes)
add_subdirectory (demos/blending)
add_subdirectory (demos/fractal2d)
add_subdirectory (demos/stress_test)
add_subdirectory (demos/game_tron)
add_subdirectory (demos/voxel)

add_subdirectory (tests/obj_model)
add_subdirectory (tests/ray_isect)