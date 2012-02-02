#include "shapes.h"

std::vector<SphereState*> SphereState::pool;
std::vector<CylinderState*> CylinderState::pool;
std::vector<PlaneState*> PlaneState::pool;
std::vector<CubeState*> CubeState::pool;



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
	transform = glm::scale( transform, glm::vec3(radius) );

	int worldLoc = shader.GetVariable("worldMatrix");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

	int locEmissive = shader.GetVariable("emissiveColor");
	shader.SetVec3(locEmissive, emissiveColor);

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

	int locEmissive = shader.GetVariable("emissiveColor");
	shader.SetVec3(locEmissive, emissiveColor);

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

	plane.init();
	GetError();

	return true;
}
