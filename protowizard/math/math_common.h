#ifndef __MATH_COMMON__H__
#define __MATH_COMMON__H__

#include <glm/glm.hpp>

#define TWO_PI     6.28318530717958647692f
#define M_PI       3.14159265358979323846f

const glm::mat4 identityMatrix(1.f);

template <class T> T DEGREES_TO_RADIANS(T degrees)
{
	return degrees * T(M_PI) / T(180.f);
}

template <class T> T RADIANS_TO_DEGREES(T radians)
{
	return radians / T(M_PI) * T(180.f);
}

#define lerp(t, a, b) ( a + t * (b - a) )


#endif
