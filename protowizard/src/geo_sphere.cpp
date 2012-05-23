#include "../proto/geo_sphere.h"

#include "../proto/math/math_common.h"
#include "../proto/opengl_stuff.h"
#include "../proto/gl_vbo.h"
#include "../proto/vertex_types.h"
#include <vector>

using namespace protowizard;

VBOPtr vbo;
unsigned int sphereVAO;
int numVertices;

unsigned int sphereIBO;
int numIndices;

typedef unsigned int uint;

struct face_t{
	uint v1;
	uint v2;
	uint v3;
};

void subdivide( std::vector<glm::vec3> &verts, std::vector<face_t> &faces)
{
	size_t numFaces = faces.size();
	for(unsigned int faceIdx=0; faceIdx<numFaces; faceIdx++) {
		uint v1 = faces[faceIdx].v1;
		uint v2 = faces[faceIdx].v2;
		uint v3 = faces[faceIdx].v3;
		glm::vec3 a = verts[v1];
		glm::vec3 b = verts[v2];
		glm::vec3 c = verts[v3];
		// Create three new verts at midpoints of each edge
		verts.push_back( glm::normalize(a+b) );
		verts.push_back( glm::normalize(b+c) );
		verts.push_back( glm::normalize(a+c) );
		// Split current tri into four
		uint i = verts.size() - 3;
		uint j = i+1;
		uint k = i+2;
		face_t f1 = {i,j,k};
		face_t f2 = {v1,i,k};
		face_t f3 = {i,v2,j};
		face_t f4 = {k,j,v3};
		faces.push_back( f1 );
		faces.push_back( f2 );
		faces.push_back( f3 );
		faces[faceIdx] = f4; // rewire current
	}
}

	
//// Num tris: levels*240 (240 = 4 * 20 * 3 verts pr triangle)
//void subdivide(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int depth, std::vector<PNVertex> &vertbuf) 
//{ 
//	if(depth == 0)
//	{
//		// actually create a triangle
//		// could crate a hash/unique inx pr vertex, and refer to it, when created again. Then create indices.
//		vertbuf.push_back( PNVertex(v3, glm::normalize(v3) ));
//		vertbuf.push_back( PNVertex(v2, glm::normalize(v2) ));
//		vertbuf.push_back( PNVertex(v1, glm::normalize(v1) ));
//		return;
//	}
//
//	glm::vec3 v12 = glm::normalize(v1 + v2);
//	glm::vec3 v23 = glm::normalize(v2 + v3);
//	glm::vec3 v31 = glm::normalize(v3 + v1);
//
//	subdivide(v1,v12,v31, depth-1, vertbuf);
//	subdivide(v2,v23,v12, depth-1, vertbuf);
//	subdivide(v3,v31,v23, depth-1, vertbuf);
//	subdivide(v12,v23,v31, depth-1, vertbuf);
//}

bool SphereGeometry::init()
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
		0,4,1, 0,9,4, 9,5,4, 4,5,8, 4,8,1,
		8,10,1, 8,3,10, 5,3,8, 5,2,3, 2,7,3,    
		7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
		6,1,10, 9,0,11, 9,11,2, 9,2,5, 7,2,11   
	};

	int subDivDepth = 2; // 2 nice! 1 isn't bad, 0 gives a icosahedron
	
	std::vector<glm::vec3> verts;
	std::vector<face_t> faces;
	for (int i = 0; i < 20; i++)
	{ 
		glm::vec3 v1(vertexData[ tindices[i][0] ][0],  vertexData[ tindices[i][0] ][1],  vertexData[ tindices[i][0] ][2] ); 
		glm::vec3 v2(vertexData[ tindices[i][1] ][0],  vertexData[ tindices[i][1] ][1],  vertexData[ tindices[i][1] ][2] );
		glm::vec3 v3(vertexData[ tindices[i][2] ][0],  vertexData[ tindices[i][2] ][1],  vertexData[ tindices[i][2] ][2] );
		verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
		int id = i*3;
		face_t face = {id+2, id+1, id+0};
		faces.push_back(face);
	}
	for (int i=0; i<subDivDepth; i++){
		subdivide( verts, faces );
	}

	std::vector<PNVertex> sphere_vertices;
	for(auto it=verts.begin(); it!=verts.end(); ++it) {
		glm::vec3 v = *it;
		bool isUnit = fabs( glm::length(v) - 1.f ) < 1e-6f;
		sphere_vertices.push_back( PNVertex(v, glm::normalize(v)) );
	}

	numVertices = sphere_vertices.size();
	const unsigned int buffer_size = sizeof(PNVertex) * sphere_vertices.size();

	// VAOs allocation & setup
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	glGenBuffers(1, &sphereIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIBO);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face_t)*faces.size(), (GLvoid*)&faces[0], GL_STATIC_DRAW);
	numIndices = 3*faces.size();

	vbo = std::shared_ptr<VBO>( new VBO(buffer_size, GL_STATIC_DRAW) );
	vbo->bind();
	vbo->buffer<PNVertex>( sphere_vertices );

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( PNVertex ), 0);
#define BUFFER_OFFSET(p) ((char*)0 + (p))
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof( PNVertex ), BUFFER_OFFSET(12) ); 

	vbo->unbind();

	return true;
}

void SphereGeometry::shutdown()
{
	glDeleteVertexArrays( 1, &sphereVAO );
	glDeleteVertexArrays( 1, &sphereIBO );
	vbo.reset();
}
	
void SphereGeometry::draw() 
{
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr );
	
#ifdef _DEBUG
	GetError();
#endif
}

