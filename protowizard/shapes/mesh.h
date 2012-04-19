#ifndef _MESH_H
#define _MESH_H

#include "../common.h"

#include <string>
#include <map>

// Create drawable VBO from VertexArray
class Mesh
{
public:
	void draw();
	Mesh( std::vector<Vertex_VNC>& verts );
	Mesh( std::vector<Vertex_VNT>& verts );
	~Mesh();


private:
	GLuint vbo;
	GLuint vao;
	int num_vertices;
};

#endif