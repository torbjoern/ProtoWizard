#pragma once

#include <string>
#include <map>
#include <vector>

#include "../vertex_types.h"

// Create drawable VBO from VertexArray
namespace protowizard{
	class Mesh
	{
	public:
		void draw( bool isTwoSided );
		Mesh( std::vector<Vertex_VNC>& verts );
		Mesh( std::vector<Vertex_VNT>& verts );
		Mesh( size_t nverts, glm::vec3* verts );
		~Mesh();


	private:
		unsigned int vbo;
		unsigned int vao;
		int num_vertices;
	};
}

