#include "proto/shapes/mesh.h"

#include "proto/vertex_types.h"


Mesh::~Mesh()
{
	glDeleteBuffers( 1, &vbo );	
	glDeleteVertexArrays(1, &vao );
}

void Mesh::draw()
{
	glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, num_vertices );
	glBindVertexArray(0);
}

Mesh::Mesh( std::vector<Vertex_VNC>& verts ) 
{
	this->num_vertices = (int)verts.size();
	int num_tris = num_vertices / 3;
	vbo = vao = 0;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_VNC) * num_vertices, &verts[0], GL_STATIC_DRAW);
	if (glGetError() == GL_OUT_OF_MEMORY)
	{
		std::runtime_error("fatal, out of GL memory");
	}

	// XYZ vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNC ), 0);

#define BUFFER_OFFSET(p) ((char*)0 + (p))
	// we use buffer offset to set stride
	// stride for the format pos & normal assuming pos is xyz and normal is nxnynz, aka 3 floats in each
	// 3 floats * 4 bytes pr float = 12 bytes, means stride = 12

	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNC ), BUFFER_OFFSET( 12 ) );

	// colors
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNC ), BUFFER_OFFSET( 12 ) );

#undef BUFFER_OFFSET


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::Mesh( std::vector<Vertex_VNT>& verts ) 
{
	this->num_vertices = (int)verts.size();
	int num_tris = num_vertices / 3;
	vbo = vao = 0;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_VNT) * num_vertices, &verts[0], GL_STATIC_DRAW);
	if (glGetError() == GL_OUT_OF_MEMORY)
	{
		std::runtime_error("fatal err: GL_OUT_OF_MEMORY");
	}

	// XYZ vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNT ), 0);

#define BUFFER_OFFSET(p) ((char*)0 + (p))
	// we use buffer offset to set stride
	// stride for the format pos & normal assuming pos is xyz and normal is nxnynz, aka 3 floats in each
	// 3 floats * 4 bytes pr float = 12 bytes, means stride = 12

	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNT ), BUFFER_OFFSET( 12 ) );

	// texcoords (s,t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex_VNT ), BUFFER_OFFSET( 24 ) );

#undef BUFFER_OFFSET


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
