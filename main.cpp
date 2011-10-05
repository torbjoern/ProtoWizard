
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
	float radius = 1.0f;

	float range = +25.0f;
	for(int i=0; i<cones; i++)
	for(int j=0; j<cones; j++)
	{
		float x = i/(float) (cones-1);
		float y = 0.f;
		float z = j/(float) (cones-1);

		protoGraphics.setColor( x,y,z );
		protoGraphics.drawCone( glm::vec3(-range * .5f) + glm::vec3(x,0.f,z) * range, radius,  
			glm::vec3(-range * .5f) + glm::vec3(x,1.f,z) * range, radius  );
	}

	for(int i=0; i<cones; i++)
	for(int j=0; j<cones; j++)
	{
		float x = i/(float) (cones-1);
		float y = j/(float) (cones-1);
		float z = 0.f;

		protoGraphics.setColor( x,y,z );
		protoGraphics.drawCone( glm::vec3(-range * .5f) + glm::vec3(x,y,0.f) * range, radius,  
			glm::vec3(-range * .5f) + glm::vec3(x,y,1.f) * range, radius  );
	}

	for(int i=0; i<cones; i++)
	for(int j=0; j<cones; j++)
	{
		float x = 0.f;
		float y = i/(float) (cones-1);
		float z = j/(float) (cones-1);

		protoGraphics.setColor( x,y,z );
		protoGraphics.drawCone( glm::vec3(-range * .5f) + glm::vec3(0.f,y,z) * range, radius,  
			glm::vec3(-range * .5f) + glm::vec3(1.f,y,z) * range, radius  );
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

	int balls = 25;
	std::vector<char> do_draw(balls*balls*balls);
	
	
	for(int i=0; i<balls; i++)
	{
		for(int j=0; j<balls; j++)
			for(int k=0; k<balls; k++)
			{
				float x = i/(float) (balls-1);
				float y = j/(float) (balls-1);
				float z = k/(float) (balls-1);
				do_draw[i+j*balls+k*balls*balls] = protoGraphics.octaves_of_noise( 4, x, y, z) < 0.0 ? char(0) : char(1);
			}
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
		//float range = +25.0f;

		//float ball_radius = 0.5f * (2*range / (balls-1) );

		//for(int i=0; i<balls; i++)
		//{
		//	for(int j=0; j<balls; j++)
		//	{
		//		for(int k=0; k<balls; k++)
		//		{
		//			float x = i/(float) (balls-1);
		//			float y = j/(float) (balls-1);
		//			float z = k/(float) (balls-1);

		//			//protoGraphics.setColor( x,y,z );


		//			if ( do_draw[i+j*balls+k*balls*balls] == 1 ){
		//				//protoGraphics.drawSphere( glm::vec3(-range * .5f) + glm::vec3(x,y,z) * range, ball_radius );
		//				protoGraphics.drawCube( glm::vec3(-range * .5f) + glm::vec3(x,y,z) * range, ball_radius );
		//			}

		//			//protoGraphics.setColor( 1.f, 1.f, 1.f );
		//			//protoGraphics.drawCube( glm::vec3(-range * .5f) + glm::vec3(x,y,z) * range, 5.f );
		//		}
		//	}
		//}

		//////////////////////////////////////////////////////////////////////////

		
		protoGraphics.setBlend( true );
		protoGraphics.drawCone( glm::vec3(0.f, 15.f, 0.f), 25.0f, glm::vec3(0.f, -15.f, 0.f), 25.f );
		draw_cone_grid(protoGraphics, 6);
		protoGraphics.setBlend( false );


		

		//protoGraphics.drawCone( glm::vec3(0.f, 10.f, 0.f), 1.0f, glm::vec3(0.f, -10.f, 0.f), 1.f );
		//protoGraphics.drawCone( glm::vec3(-10.f, 0.f, 0.f), 1.0f, glm::vec3(10.f, 0.f, 0.f), 1.f );
		//protoGraphics.drawCone( glm::vec3(0.f, 0.f, -10), 1.0f, glm::vec3(0.f, 0.f, 10.f), 1.f );
		//

		for (int i = 0; i<8; i++)
		{
			float ang = i * TWO_PI / 8;
			float r = -25.f;
			spiky( protoGraphics, glm::vec3(cos(ang)*r, 0.f, sin(ang)*r), time*pow(-1.0f, (float)i) );
		}
		

		protoGraphics.setColor(1.f, 0.f, 0.f);
		protoGraphics.drawCircle( (float) protoGraphics.getMouseX(),(float) protoGraphics.getMouseY(), 10.0f + (0.5f + sin(time*16)*0.5f ) *5.f );

		//protoGraphics.setColor(1.f, 1.f, 0.f);
		//protoGraphics.moveTo( 0.f, 0.f );
		//protoGraphics.setColor(0.f, 1.f, 0.f);
		//protoGraphics.lineTo( 400.f, 400.f );

		////////////////////////////////////////////////////////////////////////// Line speed test
		//float xres = (float)protoGraphics.getWindowWidth();
		//float yres = (float)protoGraphics.getWindowHeight();
		//
		//protoGraphics.setColor( 1.f, 1.f, 0.f );
		//for(int i=0; i<10000; i++)
		//{
		//	int x = i;
		//	x = (x<<13) ^ x;
		//	float noise1 = ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		//	x = (i<<13) ^ (i+1);
		//	float noise2 = ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		//	protoGraphics.moveTo(xres*0.5f, yres*0.5f );
		//	protoGraphics.lineTo( xres*0.5f + (xres*.5f) * noise1, yres*0.5f + (yres*.5f) * noise2 );
		//}

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


}