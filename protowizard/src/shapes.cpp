#include <proto/shapes/shapes.h>
#include "proto/shapes/mesh.h"

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

	bool inited = true;
	inited &= circle.init();
	inited &= sphere.init();
	inited &= cylinder.init();
	inited &= cube.init();
	inited &= plane.init();

	if ( !inited ) {
		printf("failed to init some shape");
	}
	return inited;
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
