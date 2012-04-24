#include "mesh_manager.h"

#include <memory>

#include "shapes/mesh.h"
#include "../depends/obj_loader/objLoader.h"

class MeshManagerImplementation : public MeshManager
{
	friend MeshManager;
private:
	typedef std::shared_ptr<Mesh> MeshPtr;
	std::map< std::string, MeshPtr > mesh_map;
public:
	virtual ~MeshManagerImplementation() {
		mesh_map.clear();
	}

	virtual MeshPtr getMesh( const std::string& file_path )
	{
		if ( mesh_map[file_path] == 0x0 )
		{
			mesh_map[file_path] = createMesh( file_path );
			if ( mesh_map[file_path] == 0x0 ) 
			{
				throw std::runtime_error("cant find mesh " + file_path);
			} 
		} 
		return mesh_map[file_path];
	}

	virtual void drawMesh( const std::string& file_path )
	{
		mesh_map.find( file_path )->second->draw();
	}

private:
	virtual MeshPtr createMesh(const std::string& fileName)
	{
		// no need to keep vertices around in here... just make a VBO

		objLoader *objData = new objLoader();

		char* cstrFileName = new char[fileName.length()+1];
		strcpy_s( cstrFileName, fileName.length()+1, fileName.c_str() );
		if ( objData->load( cstrFileName ) == 0 ) 
		{
			// could not load mesh. 
			// TODO use place holder mesh. some red 3d text "PLACEHOLDER - MESH NOT FOUND"
			return std::shared_ptr<Mesh>( 0 );
		}
	
		std::vector< glm::vec3 > obj_vertex_coords;
		std::vector< glm::vec3 > obj_vertex_normals;
		std::vector< glm::vec2 > obj_texture_coords;
		std::vector< glm::vec3 > obj_diffuse_colors;

		printf("loading obj model %s\n", cstrFileName);
		printf("Number of faces: %i\n", objData->faceCount);
		printf("Number of vertices: %i\n", objData->vertexCount);
		printf("Number of vertex normals: %i\n", objData->normalCount);
		printf("Number of texture coordinates: %i\n", objData->textureCount);
		printf("Number of material definitions: %i\n", objData->materialCount);
		printf("\n");

		// TODO idiot code:
	
		bool hasNormals = objData->normalCount > 0;
		bool hasUvs = objData->textureCount > 0;
		bool hasColors = objData->materialCount > 0;

		for(int i=0; i<objData->faceCount; i++)
		{
			if ( objData->faceList[i]->vertex_count <= 2 ) 
			{
					continue; // Hack. bad data
			}
	
			obj_face *facePtr = objData->faceList[i];

			if ( objData->materialCount > 0 )
			{
				obj_material *mat = objData->materialList[ facePtr->material_index ];
				glm::vec3 color = glm::vec3( mat->diff[0], mat->diff[1], mat->diff[2] );
				obj_diffuse_colors.push_back( color );
			}
		
			obj_vector *v0 = (objData->vertexList[ facePtr->vertex_index[0] ]);
			obj_vector *v1 = (objData->vertexList[ facePtr->vertex_index[1] ]);
			obj_vector *v2 = (objData->vertexList[ facePtr->vertex_index[2] ]);
			obj_vertex_coords.push_back( glm::vec3( v0->e[0], v0->e[1], v0->e[2] ));
			obj_vertex_coords.push_back( glm::vec3( v1->e[0], v1->e[1], v1->e[2] ));
			obj_vertex_coords.push_back( glm::vec3( v2->e[0], v2->e[1], v2->e[2] ));

			obj_vector *n0 = NULL;
			obj_vector *n1 = NULL;
			obj_vector *n2 = NULL;
			if (hasNormals)
			{
				n0 = objData->normalList[ facePtr->normal_index[0] ];
				n1 = objData->normalList[ facePtr->normal_index[1] ];
				n2 = objData->normalList[ facePtr->normal_index[2] ];

				obj_vertex_normals.push_back( glm::vec3( n0->e[0],n0->e[1],n0->e[2] ));
				obj_vertex_normals.push_back( glm::vec3( n1->e[0],n1->e[1],n1->e[2] ));
				obj_vertex_normals.push_back( glm::vec3( n2->e[0],n2->e[1],n2->e[2] ));

				// Dupe color for this face

				if (objData->materialCount > 0 )
				{
					obj_material *mat = objData->materialList[ facePtr->material_index ];
					glm::vec3 color = glm::vec3( mat->diff[0], mat->diff[1], mat->diff[2] );
					obj_diffuse_colors.push_back( color );
				}
			}

			obj_vector *uv0 = NULL;
			obj_vector *uv1 = NULL;
			obj_vector *uv2 = NULL;
		
			if (hasUvs)
			{
				uv0 = objData->textureList[ facePtr->texture_index[0] ];
				uv1 = objData->textureList[ facePtr->texture_index[1] ];
				uv2 = objData->textureList[ facePtr->texture_index[2] ];

				obj_texture_coords.push_back( glm::vec2( (float) uv0->e[0], (float) uv0->e[1] ) );
				obj_texture_coords.push_back( glm::vec2( (float) uv1->e[0], (float) uv1->e[1] ) );
				obj_texture_coords.push_back( glm::vec2( (float) uv2->e[0], (float) uv2->e[1] ) );
			}


			// Convert quads to tris
			// Load vert 3, and create new tri in order 0,2,3
			if ( objData->faceList[i]->vertex_count == 4 )
			{
				obj_vector *v3 = (objData->vertexList[ facePtr->vertex_index[3] ]);

				obj_vertex_coords.push_back( glm::vec3( v0->e[0],v0->e[1],v0->e[2] ));
				obj_vertex_coords.push_back( glm::vec3( v2->e[0],v2->e[1],v2->e[2] ));
				obj_vertex_coords.push_back( glm::vec3( v3->e[0],v3->e[1],v3->e[2] ));

				if ( hasNormals )
				{
					obj_vector *n3 = objData->normalList[ facePtr->normal_index[3] ];
					obj_vertex_normals.push_back( glm::vec3( n0->e[0],n0->e[1],n0->e[2] ));
					obj_vertex_normals.push_back( glm::vec3( n2->e[0],n2->e[1],n2->e[2] ));
					obj_vertex_normals.push_back( glm::vec3( n3->e[0],n3->e[1],n3->e[2] ));
				}

				if (hasUvs )
				{
					obj_vector *uv3 = objData->textureList[ facePtr->texture_index[3] ];

					obj_texture_coords.push_back( glm::vec2(uv0->e[0], uv0->e[1]) );
					obj_texture_coords.push_back( glm::vec2(uv2->e[0], uv2->e[1]) );
					obj_texture_coords.push_back( glm::vec2(uv3->e[0], uv3->e[1]) );
				}
			
			}

		}

		delete(objData);
	
		if( !hasNormals )
		{
			// Calc face normals
			for (size_t i=0; i<obj_vertex_coords.size(); i+=3)
			{
				glm::vec3 a = obj_vertex_coords[i+0] ;
				glm::vec3 b = obj_vertex_coords[i+1] ;
				glm::vec3 c = obj_vertex_coords[i+2] ;
				glm::vec3 ba = a-b;
				glm::vec3 bc = c-b;
				glm::vec3 normal = glm::normalize( glm::cross( ba, bc ) );
				// Per-face normals. Dupe info
				obj_vertex_normals.push_back( normal );
				obj_vertex_normals.push_back( normal );
				obj_vertex_normals.push_back( normal );
			}
		}

	

		// Just vertices and optionally colors, if mesh has no colors, set all to white
		if ( !hasUvs )
		{
			std::vector< Vertex_VNC > vertBuf;

			for (size_t i=0; i<obj_vertex_coords.size(); i++)
			{
				glm::vec3 color( 1.f );
				if (hasColors)
				{
					color = obj_diffuse_colors[ (obj_vertex_coords.size()-1-i)/6];
				}
			
				glm::vec3 &pos = obj_vertex_coords[obj_vertex_coords.size()-1-i];
				glm::vec3 &normal = obj_vertex_normals[obj_vertex_coords.size()-1-i];
				vertBuf.push_back( Vertex_VNC( pos, normal, color) );
			}

			MeshPtr m = std::shared_ptr<Mesh>( new Mesh(vertBuf) );
			return m;
		} else {
			std::vector< Vertex_VNT > vntList;

			for (size_t i=0; i<obj_vertex_coords.size(); i++)
			{
				vntList.push_back( Vertex_VNT(obj_vertex_coords[i], obj_vertex_normals[i], obj_texture_coords[i]) );
			}
			MeshPtr m = std::shared_ptr<Mesh>( new Mesh(vntList) );
			return m;
		}
	
		return 0x0;

	}
};

MeshManager *MeshManager::init()
{
	return new MeshManagerImplementation;
}
void MeshManager::shutdown(MeshManager *tm)
{
	delete tm;
}

