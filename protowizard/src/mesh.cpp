#include "proto/shapes/mesh.h"

#include "proto/vertex_types.h"
#include "proto/gl_attrib.h"
#include "proto/gl_vao.h"
#include "proto/gl_vbo.h"
#include "proto/gl_ibo.h"

using namespace protowizard;

#include "../depends/gl3w/gl3w.h"

#include <glm/ext.hpp>

void Mesh::draw()
{
	if ( isTwoSided ) { glDisable(GL_CULL_FACE); }
	
	vao->bind();
	if ( ibo ) {
		glDrawElements(GL_TRIANGLES, ibo->size(), GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, num_vertices );
	}
	glBindVertexArray(0);
	if ( isTwoSided ) { glEnable(GL_CULL_FACE); }
}

void Mesh::calcDimensions(const MeshData_t &meshData)
{
	centroid = glm::vec3(0.f);
	glm::vec3 bounds_min(0.f);
	glm::vec3 bounds_max(0.f);

	for(size_t i=0; i<meshData.vertices.size(); i++){
		const glm::vec3 &vert = meshData.vertices[i];
		centroid += vert;

		for ( int dim=0; dim<3; dim++){
			if ( vert[dim] < bounds_min[dim] ) {
				bounds_min[dim]= vert[dim];
			}
		
			if ( vert[dim] > bounds_max[dim] ) {
				bounds_max[dim] = vert[dim];
			}
		}
	}
	centroid /= (float)meshData.vertices.size();
	dimensions = glm::abs(bounds_max - bounds_min);
}

Mesh::Mesh( MeshData_t &meshData )
{
	calcDimensions(meshData);
	isTwoSided = false;
	this->num_vertices = (int)meshData.vertices.size();
	this->num_triangles = (int)meshData.indices.size() / 3;
	this->num_indices = (int)meshData.indices.size();
	vao = VAOPtr( new VAO );
	vbo = VBOPtr( new VBO(meshData.getBufferSize(), GL_STATIC_DRAW) );
	if ( meshData.indices.size() )  ibo = IBOPtr( new IBO(meshData.indices, GL_STATIC_DRAW) );

	
	std::vector<GLsizeiptr> offsets;
	std::vector<GLint> elemSizes;
#define countOf(T) ( sizeof(T)/sizeof(T[0]) )
	offsets.push_back( vbo->buffer<glm::vec3>(meshData.vertices) ); elemSizes.push_back( (countOf(meshData.vertices[0])) );
	if (meshData.hasNormals())    { offsets.push_back( vbo->buffer<glm::vec3>(meshData.normals) );   elemSizes.push_back(countOf(meshData.normals[0])); }
	if (meshData.hasTangents())   { offsets.push_back( vbo->buffer<glm::vec3>(meshData.tangents) );  elemSizes.push_back(countOf(meshData.tangents[0])); }
	if (meshData.hasBitangents()) { offsets.push_back( vbo->buffer<glm::vec3>(meshData.bitangents) );elemSizes.push_back(countOf(meshData.bitangents[0])); }
	if (meshData.hasTexCoords())  { offsets.push_back( vbo->buffer<glm::vec2>(meshData.texcoords) ); elemSizes.push_back(countOf(meshData.texcoords[0])); }
	if (meshData.hasColors())     { offsets.push_back( vbo->buffer<glm::vec4>(meshData.colors) );    elemSizes.push_back(countOf(meshData.colors[0])); }
#undef countOf

	for (size_t i=0; i<offsets.size(); i++){
		GLuint loc = i;
		//glVertexAttribPointer( attIdx, elementSize, dataType, isNormalized, stride, ptrOffset ) 
		glVertexAttribPointer(loc, elemSizes[i], GL_FLOAT, GL_FALSE, 0, (char*)0 + offsets[i]);
		glEnableVertexAttribArray(loc);
	}

	vao->unbind();
	vbo->unbind();
	ibo->unbind();
}
