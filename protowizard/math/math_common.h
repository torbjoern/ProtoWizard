#ifndef __MATH_COMMON__H__
#define __MATH_COMMON__H__

#define GLM_SWIZZLE 
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp> // Value ptr -> column-ordered pointer to GLM type

#define TWO_PI     6.28318530717958647692f
#define M_PI       3.14159265358979323846f

const glm::mat4 identityMatrix(1.f);
const glm::vec3 unitX( 1.f, 0.f, 0.f );
const glm::vec3 unitY( 0.f, 1.f, 0.f );
const glm::vec3 unitZ( 0.f, 0.f, 1.f );

template <class T> T DEGREES_TO_RADIANS(T degrees)
{
	return degrees * T(M_PI) / T(180.f);
}

template <class T> T RADIANS_TO_DEGREES(T radians)
{
	return radians / T(M_PI) * T(180.f);
}
//// given to points p1 and p2 create a vector out
//// that is perpendicular to (p2-p1)
//glm::vec3 createPerp(const glm::vec3& p1, const glm::vec3& p2)
//{
//	glm::vec3 invec = glm::normalize(p2 - p1);
//	glm::vec3 out = glm::cross( invec, glm::vec3(0.f, 0.f, 1.f) );
//	if ( glm::length(out) == 0 )
//	{
//		out = glm::cross( invec, glm::vec3(0.f, 1.f, 0.f) );
//	}
//	return out;
//}

#endif
