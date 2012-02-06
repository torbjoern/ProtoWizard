#ifndef _MESH_H
#define _MESH_H

#include "../common.h"

#include <string>
#include <map>

// a mesh consisting that consists of VNT (vertex_coord, normal, tex_coord) vertices.
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

// TODO. make dtor run. remove static funcs
class MeshManager
{
public:

	~MeshManager()
	{
		for( auto it = begin(mesh_map); it!=end(mesh_map); ++it){
			delete it->second;
		}
	}

	// draws a 3D mesh at pos with (horizontal ,vertical) orientation
	// if the mesh couln't be found, it draws a placeholder... some error register should indicate this
	static void draw( const std::string& file_path );

	static Mesh* createMesh(const std::string& fileName);
private:
	static std::map<std::string, Mesh*> mesh_map;

};

#endif