#include "proto/gl_vao.h"
#include "../depends/gl3w/gl3w.h"

using namespace protowizard;

VAO::VAO()
{
	glGenVertexArrays(1, &handle);
	bind();
}

VAO::~VAO()
{
	glDeleteVertexArrays(1, &handle);
}

void VAO::bind()
{
	glBindVertexArray(handle);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}
