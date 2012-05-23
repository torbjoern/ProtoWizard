#pragma once

#include "../depends/gl3w/gl3w.h"

#include <memory>
#include <vector>

namespace protowizard
{
	class VBO;
	typedef std::shared_ptr<VBO> VBOPtr;
	
	class VBO
	{
	public:
		VBO(const unsigned int &size, const unsigned int &draw_type);
		~VBO();

		void bind();
		void unbind();

		template<class T>
		unsigned int buffer(const std::vector<T> &data)
		{
			// http://www.opengl.org/sdk/docs/man4/xhtml/glBufferSubData.xml
			glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(T)*data.size(), &data[0]);
			if (glGetError() == GL_OUT_OF_MEMORY) {
				std::runtime_error("fatal, out of GL memory");
			}
			unsigned int return_offset = offset;
			offset += sizeof(T)*data.size();
			return return_offset;
		}

	private:
		unsigned int handle;
		unsigned int offset;
	};
}
