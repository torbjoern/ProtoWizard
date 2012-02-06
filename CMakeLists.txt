cmake_minimum_required (VERSION 2.6) 
project (PROTOWIZARD) 

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

set(CMAKE_MODULE_PATH F:/gitrepos/cmake/modules)

find_package(OPENGL REQUIRED)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

find_package(SOIL REQUIRED)
include_directories(${SOIL_INCLUDE_DIR})

add_subdirectory (protowizard) 
add_subdirectory (depends/obj_loader)
add_subdirectory (demos/simple_3d_shapes)
add_subdirectory (demos/blending)