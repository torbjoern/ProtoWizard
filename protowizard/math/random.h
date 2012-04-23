#pragma once;

#include <glm/glm.hpp>

namespace protowizard{
	double rnd();
	float sfrand();
	float random( float range_begin, float range_end );
	glm::vec3 randPointOnSphere(float radius);
	glm::vec3 randomUnitVector();
} // end namespace
