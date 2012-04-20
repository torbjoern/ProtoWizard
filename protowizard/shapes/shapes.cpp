#include "shapes.h"
#include "mesh.h"

#include <algorithm>

namespace Shapes{
LineGeometry line;
CircleGeometry circle;
CylinderGeometry cylinder;
SphereGeometry sphere;
CubeGeometry cube;
PlaneGeometry plane;
}

bool Shapes::init()
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
	return true;
}

void Shapes::de_init()
{
	circle.shutdown();
	line.shutdown();
	sphere.shutdown();
	cube.shutdown();
	cylinder.shutdown();
	plane.shutdown();
}

void MeshState::draw()
{
	mesh->draw();
}

void SphereState::pre_draw(Shader const& shader)
{
	setBlendMode();
	int worldLoc = shader.GetVariable("worldMatrix");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

	int isSphere = shader.GetVariable("isSphere");
	shader.SetInt(isSphere, 1);
}

void SphereState::draw()
{
	Shapes::sphere.draw();
}

//////////////

void CylinderState::pre_draw(Shader const& shader)
{
	setBlendMode();
	int worldLoc = shader.GetVariable("worldMatrix");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

	int isSphere = shader.GetVariable("isSphere");
	shader.SetInt(isSphere, 0);
}

void CylinderState::draw()
{
	Shapes::cylinder.draw( hasCap );
}
