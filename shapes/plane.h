#ifndef _PLANE_H
#define _PLANE_H

#include "../common.h"

class Plane
{
	public:

	Plane()
	{
		planeVAO = 0;
		planeBufferObject = 0;
		num_vertices = 0;
	}

	void shutdown()
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
		glDeleteBuffers( 1, &planeBufferObject);
		glDeleteVertexArrays( 1, &planeVAO );
	}

	void createGeometry( glm::vec3 pos, glm::vec3 normal, float radius )
	{
		glm::vec3 not_normal = normal;

		glm::vec3 perp = normal;
		float eps = 1e-7f;
		if ( fabs(not_normal.x) < eps && fabs(not_normal.z) < eps){ // comparing to eps instead of bla == 0
			not_normal.x += 1.0f;
		}else{
			not_normal.y += 1.0f;
		}

		glm::vec3 a = glm::normalize( glm::cross(perp,not_normal) );
		glm::vec3 b = glm::cross(perp,a);

		float half_size = radius * 0.5f;
		glm::vec3 u0( -a*half_size-b*half_size );
		glm::vec3 u1( -a*half_size+b*half_size );
		glm::vec3 u2( a*half_size+b*half_size );
		glm::vec3 u3( a*half_size-b*half_size );

		u0 += pos;
		u1 += pos;
		u2 += pos;
		u3 += pos;

		// TODO store in class, not here... better memory behaviour
		std::vector< PNVertex > vertices;

		vertices.push_back( PNVertex(u0,normal) );
		vertices.push_back( PNVertex(u1,normal) );
		vertices.push_back( PNVertex(u2,normal) );

		vertices.push_back( PNVertex(u3,normal) );
		vertices.push_back( PNVertex(u0,normal) );
		vertices.push_back( PNVertex(u2,normal) );
		
		num_vertices = vertices.size();

		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PNVertex) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
	}
	
	bool init()
	{
		planeBufferObject = 0;
		planeVAO = 0;

		glGenVertexArrays( 1, &planeVAO );
		glBindVertexArray(planeVAO);
		glGenBuffers(1, &planeBufferObject);

		// http://www.opengl.org/sdk/docs/man3/xhtml/glBindBuffer.xml
		glBindBuffer(GL_ARRAY_BUFFER, planeBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);


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
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, num_vertices );
		glBindVertexArray(0);
	}
	
	private:
	unsigned int planeBufferObject;
	unsigned int planeVAO;
	int num_vertices;
	
};



#endif
