#ifndef _SHAPES_H
#define _SHAPES_H

#include "circle.h"
#include "sphere.h"
#include "cylinder.h"

struct ColorRGBA
{
	float r,g,b,a;
};

struct CircleState
{
	float x, y, radius;
	ColorRGBA color;
};

struct SphereState
{
	float x, y, z, radius;
	ColorRGBA color;
};

struct CylinderState
{
	glm::vec3 p1;
	glm::vec3 p2;
	ColorRGBA color;
	float radius1;
	float radius2;
};



#endif
