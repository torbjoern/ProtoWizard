cmake_minimum_required (VERSION 2.6)

set(HDRS
	bass_wrapper.h

)

set(SRCS
	bass_wrapper.cpp
)


add_library( bass_wrapper ${HDRS} ${SRCS} )
