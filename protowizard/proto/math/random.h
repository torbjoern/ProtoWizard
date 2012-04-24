#pragma once;

#include <glm/glm.hpp>

namespace protowizard{
	double rnd();
	float sfrand();
	float random( float range_begin, float range_end );
	glm::vec3 randPointOnSphere(float radius);
	glm::vec3 randomUnitVector();
	double octaves_of_noise(int octaves, double x, double y, double z);
	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);

} // end namespace
