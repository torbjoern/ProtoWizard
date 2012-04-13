cmake_minimum_required (VERSION 2.6) 
project (PROTOWIZARD) 

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
set(CMAKE_DEBUG_POSTFIX "-debug")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

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
add_subdirectory (demos/stick_physics)


add_subdirectory (tests/obj_model)
add_subdirectory (tests/ray_isect)
add_subdirectory (tests/dda_3d)

add_subdirectory (games/ballbearing)