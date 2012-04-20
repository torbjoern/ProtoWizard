#include <protographics.h>
#include <math/math_common.h>

#include <iostream>
#include <string>

void drawArm( ProtoGraphics& proto, float angle )
{
   int segs = 30;
   double periods = 3;
   double a = 0.0;
   glm::vec3 p1( -11+20*0, 4.f * sin(periods * a), 4.f * sin(.5*M_PI + periods*a) );
   for(int i=0; i<segs; i++)
   {
      double t = i / double(segs-1);

      
      double a = TWO_PI * t;
      glm::vec3 p2( -10+20*t, 4.f * sin(periods * a), 4.f * sin(.5*M_PI + periods*a) );
	  proto.drawSphere( p2, 1.0f );
	  proto.drawCone( p1, p2, -1.0f );
	  p1 = p2;
   }
}

int main(int argc, const char* argv[])
{
	ProtoGraphics proto;

	if( !proto.init(640,480,argv) )
	{
		std::cerr << "failed to init" << std::endl;
		return -1;
	}

	float t = 0.f;
	while( proto.isWindowOpen() )
	{
		proto.cls(1,1,1);

		if ( proto.mouseDownLeft() == false )
		{
			float ang = TWO_PI * proto.getNormalizedMouseX();
			float ca = cos(ang); 
			float sa = sin(ang); 
			float radi = 15.f;

			proto.setCamera(  glm::vec3(ca*radi, 0.f, sa*radi),  glm::vec3(0.f, 0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f) );
		} else {
			// Move cam up, back and right
			proto.setCamera(  glm::vec3(-10.f, 10.f, -10.f),  glm::vec3(0.f, 0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f) );
		}

		proto.debugNormals( proto.keystatus('N') );
		
		proto.setBlend( true );
		//proto.setLightBlend();
		proto.setAlpha( 0.5f );
		//proto.setColor( 0.f, 1.f, .0f ); proto.drawSphere( glm::vec3(-2.5f, 0.f, 0.f), 5.f );
		//proto.setColor( 0.f, 0.f, 1.f ); proto.drawSphere( glm::vec3(+2.5f, 0.f, 0.f), 5.f );
		//proto.setColor( 1.f, 0.f, .0f ); proto.drawSphere( glm::vec3(+0.f, 5.f, 0.f), 5.f );

		proto.setScale( 4.f );
		glm::mat4 rotmat = glm::rotate( identityMatrix, 45.f, glm::vec3(0.f, 0.f, 1.f) );
		proto.setOrientation( rotmat );

		proto.setColor( 1.f, 0.f, .0f );
		proto.drawCube( glm::vec3(rotmat * glm::vec4(-2.5f, -2.5f, 0.f, 1.f)) );
		proto.drawCube( glm::vec3(rotmat * glm::vec4(-2.5f, +2.5f, 0.f, 1.f)) );
		proto.drawCube( glm::vec3(rotmat * glm::vec4(+2.5f, +2.5f, 0.f, 1.f)) );
		proto.drawCube( glm::vec3(rotmat * glm::vec4(+2.5f, -2.5f, 0.f, 1.f)) );

		proto.setColor( 1.f, 1.f, 1.f );
		float z = 5.0f;
		proto.drawCube( glm::vec3(rotmat * glm::vec4(-2.5f, -2.5f, z, 1.f)) );
		proto.drawCube( glm::vec3(rotmat * glm::vec4(-2.5f, +2.5f, z, 1.f)) );
		proto.drawCube( glm::vec3(rotmat * glm::vec4(+2.5f, +2.5f, z, 1.f)) );
		proto.drawCube( glm::vec3(rotmat * glm::vec4(+2.5f, -2.5f, z, 1.f)) );
		
		proto.setColor( 0.f, .5f, .8f );
		proto.setBlend( false );
		drawArm( proto, proto.getNormalizedMouseY() );

		//proto.drawCircle( 320 + 30.f * proto.noise(t), 240 + 30.f * proto.noise(t-.5f), 50.f );

		proto.frame();

		t += 1.f / 500.f;
	}
}