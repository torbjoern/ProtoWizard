#include "proto/gl_ibo.h"
#include "../depends/gl3w/gl3w.h"

using namespace protowizard;

IBO::IBO(const std::vector<unsigned int> &indices, const unsigned int &draw_type)
{
	glGenBuffers(1, &handle);
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], draw_type);
	index_size = indices.size();
}

IBO::~IBO()
{
	glDeleteBuffers(1, &handle);
}

void IBO::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
}

void IBO::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
