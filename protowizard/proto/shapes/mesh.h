#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "../vertex_types.h"

// Create drawable VBO from VertexArray

namespace protowizard{
	class Mesh;
	typedef std::shared_ptr<Mesh> MeshPtr;

	class Mesh
	{
	public:
		void draw();
		Mesh( std::vector<Vertex_VNC>& verts );
		Mesh( std::vector<Vertex_VNT>& verts );
		Mesh( size_t nverts, glm::vec3* verts );
		void setIsTwoSided(bool isTwoSided) { this->isTwoSided = isTwoSided; }
		~Mesh();


	private:
		unsigned int vbo;
		unsigned int vao;
		int num_vertices;
		bool isTwoSided;
	};
}

