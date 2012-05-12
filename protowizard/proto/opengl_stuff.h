#pragma once

// opengl_stuff.h is included by all .cpp's that need GL and GLFW

#include "../depends/gl3w/gl3w.h"
#include <GL/glfw.h>
#include "shader.h"

#include <cstdio>

inline void GetError(const char *functionName = "unknown")
{
	for ( GLenum error = glGetError( ); ( GL_NO_ERROR != error ); error = glGetError( ) )
	{
		fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);

		switch ( error )
		{
		case GL_INVALID_ENUM:      printf( "\n%s\n\n", "GL_INVALID_ENUM"      );  break; // need to use glGetStringi instead of glGetString(GL_EXTENSIONS) in GLEW
		case GL_INVALID_VALUE:     printf( "\n%s\n\n", "GL_INVALID_VALUE"     );  break;
		case GL_INVALID_OPERATION: printf( "\n%s\n\n", "GL_INVALID_OPERATION" );  break;
		//case GL_STACK_OVERFLOW:    printf( "\n%s\n\n", "GL_STACK_OVERFLOW"    );  break;
		//case GL_STACK_UNDERFLOW:   printf( "\n%s\n\n", "GL_STACK_UNDERFLOW"   );  break;
		case GL_OUT_OF_MEMORY:     printf( "\n%s\n\n", "GL_OUT_OF_MEMORY"     );  break;
		default:                                                                  break;
		}
	}
}
