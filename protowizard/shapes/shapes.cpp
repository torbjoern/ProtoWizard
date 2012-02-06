#include "shapes.h"

#include "mesh.h"

#include "../../depends/obj_loader/objLoader.h"
#include <algorithm>

std::vector<SphereState*> SphereState::pool;
std::vector<CylinderState*> CylinderState::pool;
std::vector<PlaneState*> PlaneState::pool;
std::vector<CubeState*> CubeState::pool;


void MeshState::draw( GeometryLibrary* geo_lib )
{
	geo_lib->drawMesh( this->mesh_path );
}

void *SphereState::operator new(size_t size)
{
	SphereState *ptr = 0;
	if ( pool.size() ) {
		ptr = *SphereState::pool.rbegin();
		SphereState::pool.pop_back();
	}else{
		ptr = (SphereState *)malloc( sizeof(SphereState) );
	}
	return ptr;
}

void SphereState::operator delete(void *memory)
{
	SphereState::pool.push_back( (SphereState*) memory );
}

void SphereState::pre_draw(Shader const& shader)
{
	setBlendMode();

	int worldLoc = shader.GetVariable("worldMatrix");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

	int isSphere = shader.GetVariable("isSphere");
	shader.SetInt(isSphere, 1);
}

void SphereState::draw( GeometryLibrary* geo_lib )
{
	geo_lib->sphere.draw();
}

//////////////

void *CylinderState::operator new(size_t size)
{
	CylinderState *ptr = 0;
	if ( pool.size() ) {
		ptr = *CylinderState::pool.rbegin();
		CylinderState::pool.pop_back();
	}else{
		ptr = (CylinderState *)malloc( sizeof(CylinderState) );
	}
	return ptr;
}

void CylinderState::operator delete(void *memory)
{
	CylinderState::pool.push_back( (CylinderState*) memory );
}


void CylinderState::pre_draw(Shader const& shader)
{
	setBlendMode();

	// Create a matrix that will orient cyl in desired direction
	glm::vec3 normal = glm::normalize(p2 - p1);
	glm::vec3 not_normal = normal;

	glm::vec3 perp = normal;
	float eps = 1e-7f;
	if ( fabs(not_normal.x) < eps && fabs(not_normal.z) < eps){ // comparing to eps instead of bla == 0
		not_normal.x += 1.0f;
	}else{
		not_normal.y += 1.0f;
	}

	glm::vec3 a = glm::normalize( glm::cross(perp,not_normal) );
	glm::vec3 b = glm::cross(perp,a);

	float length = glm::distance( p1, p2 );
	transform = glm::mat4( glm::vec4(radius*a, 0.f), glm::vec4(length*normal, 0.f), glm::vec4(radius*b, 0.f), glm::vec4(p1,1.f) );
		
	int worldLoc = shader.GetVariable("worldMatrix");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

	int isSphere = shader.GetVariable("isSphere");
	shader.SetInt(isSphere, 0);
}

void CylinderState::draw( GeometryLibrary* geo_lib )
{
	geo_lib->cylinder.draw( this->radius > 0 ); // draw with cap if radius not negative
}

float CylinderState::distance_from_camera( glm::vec3 const& camera_pos ) const
{
	return std::min<float>( glm::distance( p1, camera_pos ), glm::distance( p2, camera_pos ) );
}

////////////////

void *CubeState::operator new(size_t size)
{
	CubeState *ptr = 0;
	if ( pool.size() ) {
		ptr = *CubeState::pool.rbegin();
		CubeState::pool.pop_back();
	}else{
		ptr = (CubeState *)malloc( sizeof(CubeState) );
	}
	return ptr;
}

void CubeState::operator delete(void *memory)
{
	CubeState::pool.push_back( (CubeState*) memory );
}

////////////////

void *PlaneState::operator new(size_t size)
{
	PlaneState *ptr = 0;
	if ( pool.size() ) {
		ptr = *PlaneState::pool.rbegin();
		PlaneState::pool.pop_back();
	}else{
		ptr = (PlaneState *)malloc( sizeof(PlaneState) );
	}
	return ptr;
}

void PlaneState::operator delete(void *memory)
{
	PlaneState::pool.push_back( (PlaneState*) memory );
}

///////////////////////////////////////////////////////////////////////////


bool GeometryLibrary::init()
{
	// TODO.. remove this, make it so line is a static class that checks if it
	// has a VBO on draw...
	if ( !line.init() )
	{
		printf("failed to init Line VBO");
		return false;
	}

	if ( !circle.init() )
	{
		printf("failed to init Circle VBO");
		return false;
	}

	if ( !sphere.init() )
	{
		printf("failed to init Sphere VBO");
		return false;
	}

	if ( !cylinder.init() )
	{
		printf("failed to init cylinder VBO");
		return false;
	}

	if ( !cube.init() )
	{
		printf("failed to init cube VBO");
		return false;
	}

	if ( !plane.init() )
	{
		printf("failed to init plane VBO");
		return false;
	}
	
	GetError("GeometryLibrary::init");

	return true;
}

void GeometryLibrary::shutdown()
{
	circle.shutdown();
	line.shutdown();
	sphere.shutdown();
	cube.shutdown();
	cylinder.shutdown();
	plane.shutdown();

	for( auto it = begin(mesh_map); it!=end(mesh_map); ++it){
		delete it->second;
	}
}

// looks up in map to see if mesh allready is loaded,
// if not, the mesh is loaded from file_path and inserted
void GeometryLibrary::drawMesh( const std::string& file_path )
{
	using std::string;

	std::map<string, Mesh* >::iterator it;
	
	it = mesh_map.find( file_path );
	
	if ( it == mesh_map.end() )
	{
		mesh_map[file_path] = createMesh( file_path );
	} else {
		(it->second)->draw();
	}
}

Mesh* GeometryLibrary::createMesh(const std::string& fileName)
{
	// no need to keep vertices around in here... just make a VBO

	objLoader *objData = new objLoader();

	char* cstrFileName = new char[fileName.length()+1];
	strcpy_s( cstrFileName, fileName.length()+1, fileName.c_str() );
	if ( !objData->load( cstrFileName ) ) 
	{
		// could not load mesh. 
		// TODO use place holder mesh. some red 3d text "PLACEHOLDER - MESH NOT FOUND"
		throw char ("Could prolly not find .obj file");
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
				obj_vector *uv3 = NULL;
				objData->textureList[ facePtr->texture_index[3] ];

				obj_texture_coords.push_back( glm::vec2(uv0->e[0], uv0->e[1]) );
				obj_texture_coords.push_back( glm::vec2(uv2->e[0], uv2->e[1]) );
				obj_texture_coords.push_back( glm::vec2(uv3->e[0], uv3->e[1]) );
			}
			
		}

	}

	delete(objData);
	
	Mesh *m = new Mesh();

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

		if ( m->setVertexData( vertBuf ) == false)
		{	
			throw char("failed to create mesh...");
			return NULL;
		}
	} else {
		std::vector< Vertex_VNT > vntList;

		for (size_t i=0; i<obj_vertex_coords.size(); i++)
		{
			vntList.push_back( Vertex_VNT(obj_vertex_coords[i], obj_vertex_normals[i], obj_texture_coords[i]) );
		}

		if ( m->setVertexData( vntList ) == false)
		{
			throw char("failed to create mesh...");
			return NULL;
		}
	}
	
	return m;

}
