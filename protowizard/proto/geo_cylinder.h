#pragma once

namespace protowizard{
class CylinderGeometry
{
public:
	static bool init();
	static void shutdown();
	static void drawWithCap();
	static void drawNoCap();
};
}