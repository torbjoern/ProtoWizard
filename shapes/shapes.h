#ifndef _SHAPES_H
#define _SHAPES_H

#include "line.h"
#include "circle.h"
#include "sphere.h"
#include "cylinder.h"
#include "cube.h"

#include "../vertex_types.h"

struct BaseState 
{
	bool blended;
	ColorRGBA color;
};

struct CircleState : public BaseState
{
	float x, y, radius;
};

struct SphereState : public BaseState
{
	float x, y, z, radius;
};

struct CylinderState : public BaseState
{
	glm::vec3 p1;
	glm::vec3 p2;
	float radius1;
	float radius2;
};

struct CubeState : public BaseState
{
	float x, y, z, radius;
};



#endif
