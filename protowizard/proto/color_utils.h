#pragma once
#include <glm/glm.hpp>

namespace protowizard{
	void hsv_to_rgb(float H, float S, float V, float &R, float &G, float &B);
	glm::vec3 hsv2rgb(float hue, float sat, float val);
}
