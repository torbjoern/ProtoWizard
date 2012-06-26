#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "../vertex_types.h"


namespace protowizard{

	class VAO; typedef std::shared_ptr<VAO> VAOPtr;
	class VBO; typedef std::shared_ptr<VBO> VBOPtr;
	class IBO; typedef std::shared_ptr<IBO> IBOPtr;

	class Mesh;
	typedef std::shared_ptr<Mesh> MeshPtr;

	class Mesh
	{
	public:
		void draw(); // should not be exposed
		Mesh( MeshData_t &meshData );
		void setIsTwoSided(bool isTwoSided) { this->isTwoSided = isTwoSided; }
		const glm::vec3 &getDimensions() { return dimensions; }
		const glm::vec3 &getCenter() { return centroid; }
	private:
		void calcDimensions(const MeshData_t &meshData);

		VAOPtr vao;
		VBOPtr vbo;
		IBOPtr ibo;

		int num_vertices;
		int num_triangles;
		int num_indices;
		bool isTwoSided;
		glm::vec3 dimensions;
		glm::vec3 centroid;
	};
}

