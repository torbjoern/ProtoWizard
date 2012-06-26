#include "proto/mesh_manager.h"

#include <memory>
#include <ctime>

#include "proto/shapes/mesh.h"
#include "../depends/obj_loader/objLoader.h"


namespace protowizard{
namespace MeshManager{
std::map< std::string, MeshPtr > mesh_map;

void shutdown() 
{
	mesh_map.clear();
}

glm::vec2 getTexCoord(objLoader &objLoader, int index )
{
	glm::vec2 co;
	for (int dim=0; dim<2; dim++){ // xyz
		co[dim] = (float)objLoader.textureList[index]->e[dim];
	}
	return co;
}

glm::vec3 getObjVector(objLoader &objLoader, obj_vector** list, int index )
{
	glm::vec3 vertex;
	for (int dim=0; dim<3; dim++){ // xyz
		vertex[dim] = (float)list[index]->e[dim];
	}
	return vertex;
}

glm::vec3 getVertex(objLoader &objLoader, int index ) { return getObjVector(objLoader, objLoader.vertexList, index ); }
glm::vec3 getNormal(objLoader &objLoader, int index ) { return getObjVector(objLoader, objLoader.normalList, index ); }

// function: internal_efficient_load
// description: uses mesh indices. Loads normals, only if there is one normal pr vertex in objLoader
void internal_efficient_load( objLoader &objLoader, MeshData_t &meshData )
{
	for (int i=0; i<objLoader.vertexCount; i++)
	{
		meshData.vertices.push_back( getVertex(objLoader, i) );
	}

	if ( objLoader.normalCount == objLoader.vertexCount )
	{
		for (int i=0; i<objLoader.normalCount; i++)
		{
			meshData.normals.push_back( getNormal(objLoader, i) );
		}
	}

	if ( objLoader.textureCount == objLoader.vertexCount )
	{
		for (int i=0; i<objLoader.normalCount; i++)
		{
			meshData.texcoords.push_back( getTexCoord(objLoader, i) );
		}
	}

	for (int i=0; i<objLoader.faceCount; i++){
		obj_face *facePtr = objLoader.faceList[i];

		if ( facePtr->vertex_count==3 ) {
			meshData.indices.push_back( (unsigned int)facePtr->vertex_index[0] );
			meshData.indices.push_back( (unsigned int)facePtr->vertex_index[1] );
			meshData.indices.push_back( (unsigned int)facePtr->vertex_index[2] );
		} else if (facePtr->vertex_count==4 ) {
			unsigned int v0 = (unsigned int)facePtr->vertex_index[0];
			unsigned int v1 = (unsigned int)facePtr->vertex_index[1];
			unsigned int v2 = (unsigned int)facePtr->vertex_index[2];
			unsigned int v3 = (unsigned int)facePtr->vertex_index[3];
			meshData.indices.push_back(v0);
			meshData.indices.push_back(v1);
			meshData.indices.push_back(v2);

			meshData.indices.push_back(v0);
			meshData.indices.push_back(v2);
			meshData.indices.push_back(v3);
		}
	}
}

void internal_best_effort_load( objLoader &objLoader, MeshData_t &meshData )
{
	for (int i=0; i<objLoader.faceCount; i++){
		obj_face *facePtr = objLoader.faceList[i];

		if ( facePtr->vertex_count==3 ) {

			for (int coord=0; coord<3; coord++){
				meshData.vertices.push_back( getVertex(objLoader, facePtr->vertex_index[coord]) );
				if (facePtr->normal_index[coord] != -1 ) meshData.normals.push_back( getNormal(objLoader, facePtr->normal_index[coord]) );
				if (facePtr->texture_index[coord] != -1 ) meshData.texcoords.push_back( getTexCoord(objLoader, facePtr->texture_index[coord]) );
			}

		} else if (facePtr->vertex_count==4 ) {

			for (int coord=0; coord<3; coord++){
				meshData.vertices.push_back( getVertex(objLoader, facePtr->vertex_index[coord]) );
				if (facePtr->normal_index[coord] != -1 ) meshData.normals.push_back( getNormal(objLoader, facePtr->normal_index[coord]) );
				if (facePtr->texture_index[coord] != -1 ) meshData.texcoords.push_back( getTexCoord(objLoader, facePtr->texture_index[coord]) );
			}

			int triangle_two[] = {0,2,3};
			for (int coord=0; coord<3; coord++){
				int idx2 = triangle_two[coord];
				meshData.vertices.push_back( getVertex(objLoader, facePtr->vertex_index[idx2]) );
				if (facePtr->normal_index[idx2] != -1 ) meshData.normals.push_back( getNormal(objLoader, facePtr->normal_index[idx2]) );
				if (facePtr->texture_index[idx2] != -1 ) meshData.texcoords.push_back( getTexCoord(objLoader, facePtr->texture_index[idx2]) );
			}

		}
	}
	// do NOT create indices
}

void internal_load( objLoader &objLoader, MeshData_t &meshData )
{
	printf("Number of faces: %i\n", objLoader.faceCount);
	printf("Number of vertices: %i\n", objLoader.vertexCount);
	printf("Number of vertex normals: %i\n", objLoader.normalCount);
	printf("Number of texture coordinates: %i\n", objLoader.textureCount);
	printf("Number of material definitions: %i\n", objLoader.materialCount);
	printf("\n");

	if ( objLoader.normalCount == objLoader.vertexCount )
	{
		internal_efficient_load(objLoader, meshData);
		puts("using vertcount==normcount loader");
	} else {
		internal_best_effort_load(objLoader, meshData);
		puts("using best effort loader");
	}

}

MeshPtr createMesh(const std::string& fileName)
{
	char* cstrFileName = new char[fileName.length()+1];
	strcpy_s( cstrFileName, fileName.length()+1, fileName.c_str() );

	printf("loading obj model %s from disk\n", cstrFileName);
	clock_t timeStart = clock();
	objLoader objLoader;
	if ( objLoader.load(cstrFileName) == 0 ) 
	{
		// could not load mesh. 
		// TODO use place holder mesh. some red 3d text "PLACEHOLDER - MESH NOT FOUND"
		return std::shared_ptr<Mesh>( nullptr );
	}
	clock_t timeEnd = clock() - timeStart;
	double timeSpent = timeEnd / (double)CLOCKS_PER_SEC;
	printf("%f seconds used to load from disk.  \n", timeSpent);
	
	std::vector<glm::vec3> obj_vertex_coords;
	std::vector<glm::vec3> obj_vertex_normals;
	std::vector<glm::vec2> obj_texture_coords;

	MeshData_t meshData;

	puts("finished loading data into memory, now copying into internal vertex attrib lists");
	//internal_load( objLoader, obj_vertex_coords, obj_vertex_normals, obj_texture_coords );
	internal_load( objLoader, meshData );
	puts("finished copy to int, now copy to internal mesh format");
	MeshPtr m = std::shared_ptr<Mesh>( new Mesh(meshData) );
	return m;
}


MeshPtr getMesh( const std::string& file_path )
{
	if ( mesh_map[file_path] == nullptr )
	{
		mesh_map[file_path] = createMesh( file_path );
		if ( mesh_map[file_path] == nullptr ) 
		{
			throw std::runtime_error("cant find mesh " + file_path);
		} 
	} 
	return mesh_map[file_path];
}


}}