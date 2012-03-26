#ifndef _SPHERE_H
#define _SPHERE_H

#include "../common.h"

class SphereGeometry
{
	public:
	SphereGeometry(){}
	~SphereGeometry()
	{

	}

	void shutdown()
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
		glDeleteBuffers( 1, &sphereBufferObject);	
		glDeleteVertexArrays( 1, &sphereVAO );
	}
	
	void draw() 
	{
		glBindVertexArray(sphereVAO);
		glDrawArrays(GL_TRIANGLES, 0, SPHERE_VERTS);
		glBindVertexArray(0);

		GetError();
	}
	
	void subdivide(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int depth, std::vector< XYZVertex > &vertbuf) 
	{ 
		if(depth == 0)
		{
			// actually create a triangle
			vertbuf.push_back( XYZVertex(v3) );
			vertbuf.push_back( XYZVertex(v2) );
			vertbuf.push_back( XYZVertex(v1) );
			return;
		}

		glm::vec3 v12 = glm::normalize(v1 + v2);
		glm::vec3 v23 = glm::normalize(v2 + v3);
		glm::vec3 v31 = glm::normalize(v3 + v1);

		subdivide(v1,v12,v31, depth-1, vertbuf);
		subdivide(v2,v23,v12, depth-1, vertbuf);
		subdivide(v3,v31,v23, depth-1, vertbuf);
		subdivide(v12,v23,v31, depth-1, vertbuf);
	}

	bool init()
	{
		// The vertices that make up a icosahedron
		// because every value in eval is a struct or double
		// we can have a double "enum" value for initing auto arrays
		float xx = 0.525731112119133606f;
		float zz = 0.850650808352039932f;
		

		// xx and zz constants make sure the distance from the origin to any of the verts of the iscosahedron is 1.0
		float vertexData[12][3] = {    
			{-xx, 0.0, zz }  , { xx, 0.0, zz }  , { -xx, 0.0, -zz } , { xx, 0.0, -zz } ,  {  0.0, zz, xx } , 
			{0.0, zz, -xx }   , {0.0, -zz, xx }  , { 0.0, -zz, -xx }  ,{  zz, xx, 0.0 } ,  { -zz, xx, 0.0 }  , 
			{zz, -xx, 0.0 } ,  { -zz, -xx, 0.0 }   
		};

		// indices used to construct triangles by linking vertices together
		int tindices[20][3] = { 
			0,4,1,   0,9,4,   9,5,4,   4,5,8,   4,8,1,
			8,10,1,  8,3,10,  5,3,8,  5,2,3,  2,7,3,    
			7,10,3,   7,6,10,   7,11,6,   11,0,6,   0,1,6,
			6,1,10,   9,0,11,  9,11,2,   9,2,5,   7,2,11   
		};

		int subDivDepth = 1;
		std::vector< XYZVertex > sphere_vertices;
		for (int i = 0; i < 20; i++)
		{ 
			glm::vec3 v1(vertexData[ tindices[i][0] ][0],  vertexData[ tindices[i][0] ][1],  vertexData[ tindices[i][0] ][2] ); 
			glm::vec3 v2(vertexData[ tindices[i][1] ][0],  vertexData[ tindices[i][1] ][1],  vertexData[ tindices[i][1] ][2] );
			glm::vec3 v3(vertexData[ tindices[i][2] ][0],  vertexData[ tindices[i][2] ][1],  vertexData[ tindices[i][2] ][2] );
			subdivide(v1,v2,v3,subDivDepth, sphere_vertices);
		}

		sphereBufferObject = 0;
		SPHERE_VERTS = sphere_vertices.size();

		// VAOs allocation & setup
		glGenVertexArrays(1, &sphereVAO);
		glBindVertexArray(sphereVAO);
		glGenBuffers(1, &sphereBufferObject);

		glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(XYZVertex) * SPHERE_VERTS, &sphere_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer( (GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);


		glBindVertexArray(0);
		
		return true;
	}
	
	
	private:
		unsigned int sphereBufferObject;
		unsigned int sphereVAO;
		int SPHERE_VERTS;
};

#endif

