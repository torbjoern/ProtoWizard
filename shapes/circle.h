#ifndef _CIRCLE_H
#define _CIRCLE_H

#include "../common.h"

#define TWO_PI     6.28318530717958647692f

class Circle
{
public:
	Circle()
	{
		vbo_handle = 0;
		vao_handle = 0;
		num_vertices = 0;
	}
	~Circle()
	{

	}

	void shutdown()
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
		glDeleteBuffers(1, &vbo_handle);
		glDeleteVertexArrays(1, &vao_handle);
	}

	void draw()
	{
		glBindVertexArray( vao_handle );
		// http://www.opengl.org/sdk/docs/man/xhtml/glDrawArrays.xml
		glDrawArrays(GL_TRIANGLE_FAN, 0, num_vertices);
		glBindVertexArray(0);
	}

	bool init()
	{

		std::vector<glm::vec4> circle_vertices;

		glm::vec4 first( 0.0f, 0.0f, 0.0f, 0.0f );
		circle_vertices.push_back(first);

		int CIRCLE_SEGMENTS = 32;
		for (int i=0; i<CIRCLE_SEGMENTS; i++)
		{
			// Counter clockwise wound vertices
			float a = TWO_PI - ( i * TWO_PI / (CIRCLE_SEGMENTS-1) ); 
			float ca = cos(a);
			float sa = sin(a);
			glm::vec4 pos_and_texcoord( ca, sa, 0.0f, 0.0f );
			circle_vertices.push_back( pos_and_texcoord );
		}

		vbo_handle = 0;
		vao_handle = 0;

		glGenVertexArrays(1, &vao_handle);
		glBindVertexArray(vao_handle);

		// http://www.opengl.org/sdk/docs/man/xhtml/glGenBuffers.xml
		glGenBuffers(1, &vbo_handle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);

		int glm_vec_4_size = sizeof(glm::vec4);
		num_vertices = circle_vertices.size();

		// http://www.opengl.org/sdk/docs/man/xhtml/glBufferData.xml
		// can generate GL_OUT_OF_MEMORY 
		glBufferData(GL_ARRAY_BUFFER, glm_vec_4_size * num_vertices, &circle_vertices[0], GL_STATIC_DRAW);
		if (glGetError() == GL_OUT_OF_MEMORY)
		{
			return false;
		}

		// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribPointer.xml
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;
	}

private:
	unsigned int vbo_handle;
	unsigned int vao_handle;
	int num_vertices;
};

#endif
