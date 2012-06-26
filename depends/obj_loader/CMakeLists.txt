
set(kixor_obj_loader_HDRS
	list.h
	obj_parser.h
	objLoader.h
	string_extra.h
)
set(kixor_obj_loader_SRCS
	list.cpp
	obj_parser.cpp
	objLoader.cpp
	string_extra.cpp
)
SOURCE_GROUP("Header Files\\Dependency\\kixor_obj_loader" FILES ${kixor_obj_loader_HDRS})
SOURCE_GROUP("Source Files\\Dependency\\kixor_obj_loader" FILES ${kixor_obj_loader_SRCS})


add_library( obj_loader STATIC ${kixor_obj_loader_HDRS} ${kixor_obj_loader_SRCS} )
