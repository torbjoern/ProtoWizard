#ifndef _CYLINDER_H
#define _CYLINDER_H

#include "../common.h"

class Cylinder
{
	public:
	Cylinder()
	{
		num_vertices = 0;
		is_dirty = true;
	}
	
	~Cylinder()
	{

	}

	void shutdown()
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
		glDeleteBuffers( 1, &cylinderBufferObject);	
		glDeleteVertexArrays(1, &cylinderVAO);
	}

	void create_geometry( glm::vec3 p1, float r1, glm::vec3 p2, float r2 )
	{
		old_p1 = p1;
		old_p2 = p2;
		old_radius1 = r1;
		old_radius2 = r2;
		// Methods:
		// * draw a regular cone from [0,0,0] to [0,1,0] then scaled and rotated by the lookAt from a to b, How do you calculate this transform matrix?

		// * use shader raytracing to find intersection between the line with offset radius r1 & r2 from center line ab

		// * use paul bourkes method:


		glm::vec3 p1p2 = p2 - p1;

		// Create two perpendicular vectors perp and q on the plane of the disk
		glm::vec3 perp = p1p2;
		if (p1p2.x == 0 && p1p2.z == 0){
			p1p2.x += 0.1f;
		}else{
			p1p2.y += 0.1f;
		}

		glm::vec3 a = glm::cross( perp, p1p2 );
		glm::vec3 b = glm::cross( p1p2, a );
		a = glm::normalize(a);
		b = glm::normalize(b);

		std::vector< PNVertex > vertices;

		float theta1 = 0.0f;
		float theta2 = TWO_PI;

		int NFACETS = 32;

		//for (int i=NFACETS;i>=0;i--)
		for (int i=0;i<NFACETS+1;i++)
		{
			float theta = theta1 + i * (theta2 - theta1) / NFACETS;

			float ct = cos(theta);
			float st = sin(theta);
			glm::vec3 normal( ct * a + st * b);
			normal = glm::normalize(normal);

			glm::vec3 v1( p2 + r2 * normal );
			vertices.push_back( PNVertex(v1, normal) );

			glm::vec3 v2( p1 + r1 * normal );
			vertices.push_back( PNVertex(v2, normal) );
		}

		num_vertices = vertices.size();

		glBindVertexArray(cylinderVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cylinderBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PNVertex) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
		
	}
	
	// TODO could have a check to see if geometry actually changes between calls
	// and what about rotation & scaling? possibly faster than regenerating geometry for a given p1,p2,r1,r2
	// is it possible to scale r1 and r2 independantly? Could make a fast function for r1=r2 that only uses translate+scale+rotate
	// and keep this one when r1!=r2
	// or do raytracing in a shader... but how fast is that?
	void draw( glm::vec3 p1, float r1, glm::vec3 p2, float r2 )
	{
		if ( p1 != old_p1 || p2 != old_p2 || r1 != old_radius1 || r2 != old_radius2 )
		{
			is_dirty = true;
		}

		if (is_dirty)
		{
			create_geometry(p1,r1,p2,r2);
		}

		glBindVertexArray(cylinderVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, num_vertices );
		glBindVertexArray(0);
	}
	
	// TODO can this function fail?
	bool init()
	{
		cylinderBufferObject = 0;
		glGenVertexArrays(1, &cylinderVAO);
		glBindVertexArray(cylinderVAO);
		glGenBuffers(1, &cylinderBufferObject);

		glBindBuffer(GL_ARRAY_BUFFER, cylinderBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( PNVertex ), 0);
		#define BUFFER_OFFSET(p) ((char*)0 + (p))
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof( PNVertex ), BUFFER_OFFSET(12) ); 

		glBindVertexArray(0);

		return true;
	}
	
	private:
		unsigned int cylinderBufferObject;
		unsigned int cylinderVAO;
		int num_vertices;

		bool is_dirty;
		glm::vec3 old_p1;
		glm::vec3 old_p2;
		float old_radius1;
		float old_radius2;
	
};


#endif
