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
		void draw(); // should not be exposed
		Mesh( std::vector<Vertex_VNC>& verts );
		Mesh( std::vector<Vertex_VNT>& verts );
		void setIsTwoSided(bool isTwoSided) { this->isTwoSided = isTwoSided; }
		const glm::vec3 &getDimensions() { return dimensions; }
		const glm::vec3 &getCenter() { return centroid; }
		~Mesh();


	private:
		void calcDimensions(const std::vector<Vertex_VNC>& verts);
		void calcDimensions(const std::vector<Vertex_VNT>& verts);

		unsigned int vbo;
		unsigned int vao;
		int num_vertices;
		bool isTwoSided;
		glm::vec3 dimensions;
		glm::vec3 centroid;
	};
}

