
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp> // Value ptr -> column-ordered pointer to GLM type

#include "protographics.h"


#include <string>
using std::string;
#include <cassert>
#include <cstdio>
#include <fstream>



int main()
{
	ProtoGraphics protoGraphics;

	if ( protoGraphics.init(800,800) == false )
	{
		printf("Failed to init OpenGL graphics\n");
		return 0;
	}


	while( true )
	{
		protoGraphics.cls(0.0f, 0.0f, 0.0f);

		float time = protoGraphics.klock();


		protoGraphics.setAlpha( 1.0f );
		protoGraphics.setColor(1.f, 0.f, 0.f);
		protoGraphics.drawCircle( (float) protoGraphics.getMouseX(),(float) protoGraphics.getMouseY(), 10.0f + (0.5f + sin(time*16)*0.5f ) *5.f );


		protoGraphics.setColor(.1f,.1f,.1f);
		protoGraphics.drawPlane( glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), 50.0f );


		protoGraphics.setColor( 1.0f, 0.0f, 0.0f );
		protoGraphics.drawSphere( glm::vec3(10.0f, 5.f, 0.0f), 5.0f );

		protoGraphics.setAlpha( protoGraphics.getMouseX() / (float)protoGraphics.getWindowWidth() );

		
		protoGraphics.setColor( 1.0f, 0.0f, 0.0f );
		protoGraphics.drawCone( glm::vec3(10.0f, 0.f, 10.0f), 1.0f, glm::vec3(10.0f, 10.f, 10.0f), 1.0f );
		protoGraphics.setColor( 0.0f, 1.0f, 0.0f );
		protoGraphics.drawCone( glm::vec3(-10.0f, 0.f, 10.0f), 1.0f, glm::vec3(-10.0f, 10.f, 10.0f), 1.0f );
		protoGraphics.setColor( 0.0f, 0.0f, 1.0f );
		protoGraphics.drawCone( glm::vec3(-10.0f, 0.f, -10.0f), 1.0f, glm::vec3(-10.0f, 10.f, -10.0f), 1.0f );
		protoGraphics.setColor( 1.0f, 1.0f, 0.0f );
		protoGraphics.drawCone( glm::vec3(10.0f, 0.f, -10.0f), 1.0f, glm::vec3(10.0f, 10.f, -10.0f), 1.0f );
		

		protoGraphics.setColor( 1.0f, 0.0f, 0.0f ); protoGraphics.drawCube( glm::vec3( 0.0f, 2.51f, 0.0f ), 2.5f );
		protoGraphics.setBlend(true);
		protoGraphics.setColor( 0.0f, 1.0f, 0.0f ); protoGraphics.drawCube( glm::vec3( 0.0f, 2.51f, 1.1f*5.0f ), 2.5f );
		protoGraphics.setColor( 0.0f, 0.0f, 1.0f ); protoGraphics.drawCube( glm::vec3( 0.0f, 2.51f, 2.2f*5.0f ), 2.5f );
		protoGraphics.setColor( 1.0f, 1.0f, 0.0f ); protoGraphics.drawCube( glm::vec3( 0.0f, 2.51f, 3.3f*5.0f ), 2.5f );
		protoGraphics.setBlend(false);

		if ( protoGraphics.keystatus(' ') )
		{
			protoGraphics.dump_stats();
		}

		protoGraphics.frame();


		if (protoGraphics.isWindowOpen() == false) break;
	}


}