#pragma once
#include "proto/math/math_common.h"
#include "../opengl_stuff.h"

namespace protowizard{
class CubeGeometry
{
	public:

	void shutdown()
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
		glDeleteBuffers( 1, &cubeBufferObject);
		glDeleteVertexArrays( 1, &cubeVAO );
	}
	
	bool init()
	{
		// method of generating Cube Vertices taken from cube.c by Mark J. Kilgard, 1997
		// http://www.opengl.org/resources/code/samples/glut_examples/examples/cube.c
		
		glm::vec3 n[6] = {  // Normals for the 6 faces of a cube. 
			glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)
		};

		int faceIndices[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
			{0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
			{4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} 
		};


		glm::vec3 v[8];  // Will be filled in with X,Y,Z vertexes.
		float size = 0.5f; // would think diameter would be 1
		v[0].x = v[1].x = v[2].x = v[3].x = -size;
		v[4].x = v[5].x = v[6].x = v[7].x = +size;
		v[0].y = v[1].y = v[4].y = v[5].y = -size;
		v[2].y = v[3].y = v[6].y = v[7].y = +size;
		v[0].z = v[3].z = v[4].z = v[7].z = +size;
		v[1].z = v[2].z = v[5].z = v[6].z = -size;

		std::vector< PNVertex > cube_vertices;
		
		// TODO indices if I can be bothered
		for (int i=0; i < 6; i++) {
			cube_vertices.push_back( PNVertex( v[ faceIndices[i][2] ], n[i] ));
			cube_vertices.push_back( PNVertex( v[ faceIndices[i][1] ], n[i] ));
			cube_vertices.push_back( PNVertex( v[ faceIndices[i][0] ], n[i] ));													   
																	   
			cube_vertices.push_back( PNVertex( v[ faceIndices[i][0] ], n[i] ));
			cube_vertices.push_back( PNVertex( v[ faceIndices[i][3] ], n[i] ));
			cube_vertices.push_back( PNVertex( v[ faceIndices[i][2] ], n[i] ));
		}
		

		num_vertices = cube_vertices.size();
		cubeBufferObject = 0;
		glGenVertexArrays( 1, &cubeVAO );
		glBindVertexArray(cubeVAO);
		glGenBuffers(1, &cubeBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, cubeBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PNVertex) * num_vertices, &cube_vertices[0], GL_STATIC_DRAW);
		if (glGetError() == GL_OUT_OF_MEMORY)
		{
			return false;
		}


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( PNVertex ), 0);

#define BUFFER_OFFSET(p) ((char*)0 + (p))
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof( PNVertex ), BUFFER_OFFSET(12) ); 
#undef BUFFER_OFFSET


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return true;
	}
	
	void draw() 
	{
		// http://www.opengl.org/sdk/docs/man3/xhtml/glBindBuffer.xml
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, num_vertices );
		glBindVertexArray(0);
	}
	
	private:
	unsigned int cubeBufferObject;
	unsigned int cubeVAO;
	int num_vertices;
	
};
} // ::proto
