#pragma once

#include "../proto/math/math_common.h"

namespace protowizard{
class PlaneGeometry
{
	public:
		static bool init();
		static void shutdown();
		static void draw();
};
} 
