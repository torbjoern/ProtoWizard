cmake_minimum_required (VERSION 2.6)
project (ProtoWizard)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

find_package(ANGELSCRIPT REQUIRED)
include_directories(${ANGELSCRIPT_INCLUDE_DIR})
include_directories(${ANGELSCRIPT_ADDON_INCLUDE_DIR})


find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})


set(HDRS
	protographics.h
)

set(SRCS

	main.cpp
)



# add the executable
add_executable ( ProtoWizard main.cpp ${ANGELSCRIPT_INCLUDE} ${HDRS} ${SRCS} )
target_link_libraries (ProtoWizard  ${ANGELSCRIPT_LIB} )