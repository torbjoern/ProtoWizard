cmake_minimum_required (VERSION 2.6)

set(HDRS
	gl3.h
	gl3w.h
)

set(SRCS
	gl3w.c
)


add_library( gl3w ${HDRS} ${SRCS} )
