#include "../proto/geo_plane.h"

#include "../proto/opengl_stuff.h"
#include "../proto/gl_vbo.h"
#include "../proto/vertex_types.h"
#include <vector>

using namespace protowizard;

unsigned int planeBufferObject = 0;
unsigned int planeVAO = 0;
int num_vertices= 0;

void createGeometry( glm::vec3 pos, glm::vec3 normal, float radius )
{
	glm::vec3 not_normal = normal;

	glm::vec3 perp = normal;
	float eps = 1e-6f;
	if ( fabs(not_normal.x) < eps && fabs(not_normal.z) < eps){ // comparing to eps instead of bla == 0
		not_normal.x += 1.0f;
	}else{
		not_normal.y += 1.0f;
	}

	glm::vec3 a = glm::normalize( glm::cross(perp,not_normal) );
	glm::vec3 b = glm::cross(perp,a);

	float half_size = radius;
	glm::vec3 u0( -a*half_size-b*half_size );
	glm::vec3 u1( -a*half_size+b*half_size );
	glm::vec3 u2( a*half_size+b*half_size );
	glm::vec3 u3( a*half_size-b*half_size );

	glm::vec2 tex_coord0( 0.f, 0.f );
	glm::vec2 tex_coord1( 0.f, 1.f );
	glm::vec2 tex_coord2( 1.f, 1.f );
	glm::vec2 tex_coord3( 1.f, 0.f );

	// Make two triangles
	std::vector< Vertex_VNT > vertices;
	// Triangle 1
	vertices.push_back( Vertex_VNT(u0,normal,tex_coord0) );
	vertices.push_back( Vertex_VNT(u1,normal,tex_coord1) );
	vertices.push_back( Vertex_VNT(u2,normal,tex_coord2) );
	// Triangle 2
	vertices.push_back( Vertex_VNT(u3,normal,tex_coord3) );
	vertices.push_back( Vertex_VNT(u0,normal,tex_coord0) );
	vertices.push_back( Vertex_VNT(u2,normal,tex_coord2) );
		
	num_vertices = vertices.size();

	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_VNT) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}
	
bool PlaneGeometry::init()
{
	planeBufferObject = 0;
	planeVAO = 0;

	glGenVertexArrays( 1, &planeVAO );
	glBindVertexArray(planeVAO);
	glGenBuffers(1, &planeBufferObject);

	// http://www.opengl.org/sdk/docs/man3/xhtml/glBindBuffer.xml
	glBindBuffer(GL_ARRAY_BUFFER, planeBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);


#define BUFFER_OFFSET(p) ((char*)0 + (p))
	// http://www.opengl.org/sdk/docs/man3/xhtml/glVertexAttribPointer.xml
	// vertex offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNT ), 0);

	// normal offset
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNT ), BUFFER_OFFSET(3*4) ); 

	// texcoord offset
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNT ), BUFFER_OFFSET( (3+3)*4) ); 
#undef BUFFER_OFFSET

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	createGeometry( glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), 1.0f );

	return true;
}

void PlaneGeometry::shutdown()
{
	// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
	glDeleteBuffers( 1, &planeBufferObject);
	glDeleteVertexArrays( 1, &planeVAO );
}
	
void PlaneGeometry::draw() 
{
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, num_vertices );
	glBindVertexArray(0);
}
