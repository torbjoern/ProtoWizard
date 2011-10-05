#ifndef _COMMON_H
#define _COMMON_H

#define GLEW_STATIC
#include <glew.h>
#include <gl/glfw.h>
#include <cmath>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp> // Value ptr -> column-ordered pointer to GLM type

#include "vertex_types.h"
#include "shader.h"
#include "depends/vsml.h"


inline void GetError()
{
	for ( GLenum Error = glGetError( ); ( GL_NO_ERROR != Error ); Error = glGetError( ) )
	{
		switch ( Error )
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

#endif
