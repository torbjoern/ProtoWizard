#pragma once

#include "../depends/gl3w/gl3w.h"
#include <vector>

namespace protowizard
{
	class ATTRIB
	{
	public:
		static void bind(	const unsigned int &shader,
							const std::string &name,
							const unsigned int &size_per_index,
							const unsigned int &data_type,
							bool normalized,
							const unsigned int &stride,
							const unsigned int &offset,
							const unsigned int &divisor = 0)
		{
			unsigned int loc = glGetAttribLocation(shader, name.c_str());
			bind(loc, size_per_index, data_type, normalized, stride, offset, divisor);
		}

		static void bind(	const unsigned int &loc,
							const unsigned int &size_per_index,
							const unsigned int &data_type,
							bool normalized,
							const unsigned int &stride,
							const unsigned int &offset,
							const unsigned int &divisor = 0)
		{
			if(loc < 0)
				throw std::runtime_error("Couldn't find location of attribute in shader");

			glVertexAttribPointer(loc, size_per_index, data_type, normalized, stride, (GLubyte*) NULL + offset);
			glEnableVertexAttribArray(loc);
			if(divisor > 0)
				glVertexAttribDivisor(loc, divisor);
		}
	};
}