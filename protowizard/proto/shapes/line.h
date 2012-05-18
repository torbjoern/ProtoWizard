#pragma once
#include <vector>
#include "../vertex_types.h"
#include "../depends/gl3w/gl3w.h"

namespace protowizard{
class LineGeometry
{
	public:
	LineGeometry()
	{
	}
	
	~LineGeometry()
	{

	}

	void shutdown()
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
		glDeleteBuffers( 1, &LineBufferObject);	
		glDeleteVertexArrays(1, &line_vao);
	}

	
	void draw( std::vector<LineSegmentState> &lines )
	{
		int num_verts = lines.size()*2;

		glBindVertexArray( line_vao );
		glBindBuffer(GL_ARRAY_BUFFER, LineBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LineSegmentState)/2 * num_verts, &lines[0], GL_DYNAMIC_DRAW);

		glDrawArrays(GL_LINES, 0, num_verts );
		glBindVertexArray(0);

	}
	
	// TODO can this function fail?
	bool init()
	{
		LineBufferObject = 0;
		glGenVertexArrays(1, &line_vao);
		glBindVertexArray(line_vao);

		glGenBuffers(1, &LineBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, LineBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof( LineSegmentState )/2, 0);
		#define BUFFER_OFFSET(p) ((char*)0 + (p))
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof( LineSegmentState )/2, BUFFER_OFFSET( 2*4) ); 	
		#undef BUFFER_OFFSET
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);

		return true;
	}
	
	private:
		unsigned int LineBufferObject;
		unsigned int line_vao;
};
} 
