#include <protographics.h>

int main()
{
	ProtoGraphics proto;

	if (!proto.init(640,480) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}

	proto.setCamera( glm::vec3(0.f, 0.f, -15.f), 0.f, 0.f );
	size_t numframes = 0;

	proto.setFrameRate( 30 );
	do
	{
		proto.cls(0.5f, 0.5f, 0.5f);

		proto.setColor(0.f, 1.f, 0.f);
		proto.drawSphere( glm::vec3(0.f, 0.f, 0.f), 1.0f );
		proto.drawCube( glm::vec3(-5.f, 0.f, 0.f), 1.0f );
		proto.drawCone( glm::vec3(-10.f, -5.f, 0.f), glm::vec3(-10.f, 5.f, 0.f), 0.5f );
		float t1 = proto.klock();
		float t2 = t1 + 3.14f;
		proto.drawCone( glm::vec3(10.f + cos(t1)* 5.0, sin(t1) * 5.f, 0.f), glm::vec3(10.f + cos(t2) * 5.f, sin(t2) * 5.f, 0.f), 0.5f );
		proto.drawPlane( glm::vec3(5.0f, -1.f, 0.f), glm::vec3( 0.f, 1.f, 0.f ), 1.f );

		
		proto.frame();
		numframes++;
	} while( proto.isWindowOpen() );


}