
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp> // Value ptr -> column-ordered pointer to GLM type

#include "protographics.h"


#include <string>
using std::string;
#include <cassert>
#include <cstdio>
#include <fstream>

void spiky(ProtoGraphics &protoGraphics, glm::vec3 location, float angle)
{
	int SEGMENTS = 4;
	for(int i=0; i<SEGMENTS; i++)
	for(int j=0; j<SEGMENTS; j++)
	{
		float u = i / (float)(SEGMENTS) * M_PI;
		float v = j / (float)(SEGMENTS) * TWO_PI;

		float r = 5.0f;
		glm::vec3 vertex( r * sin(u) * cos(v), 
			r * sin(u) * sin(v),
			r * cos(u) );

		protoGraphics.setColor( i / (float)SEGMENTS, j / (float)SEGMENTS, 0.0f );

		glm::mat4 rotation = glm::rotate( glm::mat4(1.f), angle * 180.f / 3.14f, glm::vec3(0.f, 1.f, 0.f) );
		glm::vec4 rotated1 = rotation * glm::vec4 ( vertex, 0.f );

		protoGraphics.drawCone( glm::vec3(rotated1) + location, 0.01f, glm::vec3(0.f) + location, 1.0f );
	}
}

void draw_cone_grid(ProtoGraphics &protoGraphics, int cones)
{
	float range = +25.0f;
	for(int i=0; i<cones; i++)
	for(int j=0; j<cones; j++)
	{
		float x = i/(float) (cones-1);
		float y = 0.f;
		float z = j/(float) (cones-1);

		protoGraphics.setColor( x,y,z );
		protoGraphics.drawCone( glm::vec3(-range * .5f) + glm::vec3(x,0.f,z) * range, 0.5f,  
			glm::vec3(-range * .5f) + glm::vec3(x,1.f,z) * range, 0.5f  );
	}

	for(int i=0; i<cones; i++)
	for(int j=0; j<cones; j++)
	{
		float x = i/(float) (cones-1);
		float y = j/(float) (cones-1);
		float z = 0.f;

		protoGraphics.setColor( x,y,z );
		protoGraphics.drawCone( glm::vec3(-range * .5f) + glm::vec3(x,y,0.f) * range, 0.5f,  
			glm::vec3(-range * .5f) + glm::vec3(x,y,1.f) * range, 0.5f  );
	}

	for(int i=0; i<cones; i++)
	for(int j=0; j<cones; j++)
	{
		float x = 0.f;
		float y = i/(float) (cones-1);
		float z = j/(float) (cones-1);

		protoGraphics.setColor( x,y,z );
		protoGraphics.drawCone( glm::vec3(-range * .5f) + glm::vec3(0.f,y,z) * range, 0.5f,  
			glm::vec3(-range * .5f) + glm::vec3(1.f,y,z) * range, 0.5f  );
	}
}

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

		//float angle = time; // M_PI / 4.f
		//float r = -35.f;
		//float sa = cos( angle ) * r;
		//float ca = sin( angle ) * r;
		//protoGraphics.setCamera( glm::vec3(sa, 25.0f, ca), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f) );
		
		//////////////////////////////////////////////////////////////////////////
		float range = +250.0f;

		int balls = 10;
		for(int i=0; i<balls; i++)
		for(int j=0; j<balls; j++)
		for(int k=0; k<balls; k++)
		{
			float x = i/(float) (balls-1);
			float y = j/(float) (balls-1);
			float z = k/(float) (balls-1);

			//protoGraphics.setColor( x,y,z );
			//protoGraphics.drawSphere( glm::vec3(-range * .5f) + glm::vec3(x,y,z) * range, 0.8f );

			protoGraphics.setColor( 1.f, 1.f, 1.f );
			protoGraphics.drawCube( glm::vec3(-range * .5f) + glm::vec3(x,y,z) * range, 1.f );
		}
		//////////////////////////////////////////////////////////////////////////


		
		//draw_cone_grid(protoGraphics, 3);

		//protoGraphics.drawCone( glm::vec3(0.f, 10.f, 0.f), 1.0f, glm::vec3(0.f, -10.f, 0.f), 1.f );
		//protoGraphics.drawCone( glm::vec3(-10.f, 0.f, 0.f), 1.0f, glm::vec3(10.f, 0.f, 0.f), 1.f );
		//protoGraphics.drawCone( glm::vec3(0.f, 0.f, -10), 1.0f, glm::vec3(0.f, 0.f, 10.f), 1.f );
		//

		spiky( protoGraphics, glm::vec3(-25.f, 0.f, 0.f), time );

		protoGraphics.setColor(1.f, 0.f, 0.f);
		protoGraphics.drawCircle( (float) protoGraphics.getMouseX(),(float) protoGraphics.getMouseY(), 10.0f );

		//////////////////////////////////////////////////////////////////////////
		//int num = 100;
		//float numf = (float)num;
		//float radi = 10.f;
		//for (int i=0; i<num; i++)
		//{
		//	for(int j=0; j<num; j++)
		//	{
		//		protoGraphics.setColor( i / numf, j / numf, 0.f );
		//		protoGraphics.drawCircle( 10.f + (i * 790.f / numf), 10.f + (j * 790.f / numf), radi );
		//	}
		//	
		//}
		

		protoGraphics.frame();
		

		if (protoGraphics.isWindowOpen() == false) break;
	}

	protoGraphics.shutdown();
}