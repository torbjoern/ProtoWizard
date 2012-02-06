#ifndef _COMMON_H
#define _COMMON_H

#define TWO_PI     6.28318530717958647692f
#define M_PI       3.14159265358979323846f

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw.h>
#include <cmath>

#define GLM_SWIZZLE 
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp> // Value ptr -> column-ordered pointer to GLM type

#include "vertex_types.h"
#include "shader.h"
#include "depends/vsml.h"


inline void GetError(const char *functionName = "unknown")
{
	for ( GLenum error = glGetError( ); ( GL_NO_ERROR != error ); error = glGetError( ) )
	{
		fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);

		switch ( error )
		{
		case GL_INVALID_ENUM:      printf( "\n%s\n\n", "GL_INVALID_ENUM"      ); assert( 0 ); break; // need to use glGetStringi instead of glGetString(GL_EXTENSIONS) in GLEW
		case GL_INVALID_VALUE:     printf( "\n%s\n\n", "GL_INVALID_VALUE"     ); assert( 0 ); break;
		case GL_INVALID_OPERATION: printf( "\n%s\n\n", "GL_INVALID_OPERATION" ); assert( 0 ); break;
		case GL_STACK_OVERFLOW:    printf( "\n%s\n\n", "GL_STACK_OVERFLOW"    ); assert( 0 ); break;
		case GL_STACK_UNDERFLOW:   printf( "\n%s\n\n", "GL_STACK_UNDERFLOW"   ); assert( 0 ); break;
		case GL_OUT_OF_MEMORY:     printf( "\n%s\n\n", "GL_OUT_OF_MEMORY"     ); assert( 0 ); break;
		default:                                                                              break;
		}
	}
}

template <class T> T DEGREES_TO_RADIANS(T degrees)
{
	return degrees * T(M_PI) / T(180.f);
}

template <class T> T RADIANS_TO_DEGREES(T radians)
{
	return radians / T(M_PI) * T(180.f);
}

#define lerp(t, a, b) ( a + t * (b - a) )


#endif



