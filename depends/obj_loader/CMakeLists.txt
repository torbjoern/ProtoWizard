cmake_minimum_required (VERSION 2.6)

set(HDRS
	list.h
	obj_parser.h
	objLoader.h
	string_extra.h

)

set(SRCS
	list.cpp
	obj_parser.cpp
	objLoader.cpp
	string_extra.cpp
)


add_library( kixxor_obj_loader ${HDRS} ${SRCS} )
