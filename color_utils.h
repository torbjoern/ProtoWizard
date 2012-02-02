/*
================
hsv to rgb. Hue Saturation Value to Red Green Blue
rgb values from 0-1f
================
*/

#ifndef _COLOR_UTILS_H_
#define _COLOR_UTILS_H_

#include <glm/glm.hpp>


namespace protowizard
{
	void hsv_to_rgb(float H, float S, float V, float &R, float &G, float &B);

	glm::vec3 hsv2rgb(float hue, float sat, float val);
};

#endif
