#include <protographics.h>
#include "color_utils.h"

glm::vec2 pointOnCicle( float u )
{
	return glm::vec2( cos(u), sin(u) );
}

const float PI = 3.141592f;
const float TWO_PI = 2.f * 3.141592f;

void circle_of_things( ProtoGraphics& proto )
{
	//proto.setLightBlend();
	proto.setBlend(true);
	proto.setAlpha(0.5f);

	int num_things = 12;
	for ( int i=0; i<num_things; i++ ){
		float param = i / float(num_things);
		float ang =  param * (2.f * PI);
		glm::vec2 point = 10.f * pointOnCicle( ang ); 
		float normalized = ang/(2.f * PI);
		float hang = normalized * 360.f;

		proto.setColor( protowizard::hsv2rgb( hang, 1,1 ) ); 
		proto.setTexture("assets/textures/alpha_particle.png");
		proto.drawPlane( glm::vec3(point.x, 0.f, point.y), glm::vec3(0.f, 0.f, 1.f), 2.f );
	}
}

int main()
{
	ProtoGraphics proto;

	if (!proto.init(640,480) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}



	proto.setCamera( glm::vec3(0.f, 0.f, -15.f), 0.f, 0.f );
	size_t numframes = 0;

	glm::vec3 cam_pos(0.f, 2.f, -15.f);
	glm::vec3 cam_target = cam_pos + glm::vec3(0.f, 0.f, 1.f);


	proto.setTitle( "your friendly example project" );
	proto.setFrameRate( 60 );
	do
	{
		proto.cls(0.5f, 0.5f, 0.5f);
		
		circle_of_things( proto );
		
		proto.setBlend(false);
		proto.setAlpha(1.f);

		proto.disableTexture();
		for(int i=0; i<10; i++)
		{
			float normalized = i / 10.f;
			proto.setColor( protowizard::hsv2rgb( 360.f * normalized, 1,1 ) ); 
			//proto.setTexture("assets/textures/alpha_particle.png");
			glm::vec3 pos = glm::vec3(0.f, 0.f, normalized * 20.f);
			//proto.drawPlane( pos, glm::vec3(0.f, 0.f, 1.f), 2.f );
			proto.setScale( 0.5f + normalized * 2.f );
			proto.drawCube( pos );
		}

		
		proto.setBlend( true );
		proto.setAlpha( 0.5f );
		for(int i=0; i<10; i++)
		{
			float normalized = i / 10.f;
			proto.setColor( protowizard::hsv2rgb( 360.f * normalized, 1,1 ) ); 
			//proto.setTexture("assets/textures/alpha_particle.png");
			glm::vec3 pos = glm::vec3(-5.f, 0.f, normalized * 20.f);
			//proto.drawPlane( pos, glm::vec3(0.f, 0.f, 1.f), 2.f );
			proto.setScale( 0.5f + normalized * 2.f );
			proto.drawCube( pos );
		}

		proto.setLightBlend();
		proto.setAlpha( 0.5f );
		for(int i=0; i<10; i++)
		{
			float normalized = i / 10.f;
			proto.setColor( protowizard::hsv2rgb( 360.f * normalized, 1,1 ) ); 
			//proto.setTexture("assets/textures/alpha_particle.png");
			glm::vec3 pos = glm::vec3(+5.f, 0.f, normalized * 20.f);
			//proto.drawPlane( pos, glm::vec3(0.f, 0.f, 1.f), 2.f );
			proto.setScale( 0.5f + normalized * 2.f );
			proto.drawCube( pos );
		}
		proto.setScale(1.f);

		proto.setCamera( cam_pos, cam_target, glm::vec3(0.f, 1.f, 0.f) );
		
		glm::vec3 to_target = -cam_target;
		float vel_x = (float) ( proto.keystatus('A') - proto.keystatus('D') );
		float vel_z = (float) ( proto.keystatus('W') - proto.keystatus('S') );

		if ( fabs(vel_x) > 0.0f || fabs(vel_z) > 0.0f ) {
			float dt = proto.getMSPF();
			float vel_per_sec = 10.0f;
			if ( proto.keystatus('M') ) vel_per_sec *= 2.f;

			glm::vec3 cam_delta( vel_x * vel_per_sec * dt, 0.f, vel_z * vel_per_sec * dt );
			cam_pos += cam_delta;
			cam_target += cam_delta;
		}

		proto.frame();
		numframes++;
	} while( proto.isWindowOpen() );


}