#ifndef _MESH_H
#define _MESH_H

#include "../common.h"

#include <string>
#include <map>

// a 3d triangle mesh
class Mesh
{
public:
	void draw();
	~Mesh();
	bool setVertexData( std::vector<Vertex_VNC>& verts );
	bool setVertexData( std::vector<Vertex_VNT>& verts );

private:
	GLuint vbo;
	GLuint vao;
	int num_vertices;
};

#endif