#include <protographics.h>
#include "color_utils.h"

glm::vec2 pointOnCicle( float u )
{
	return glm::vec2( cos(u), sin(u) );
}

const float PI = 3.141592f;
const float TWO_PI = 2.f * 3.141592f;

void circle_of_birds( ProtoGraphics& proto )
{
	int num_birds = 18;
	for ( int i=0; i<num_birds; i++ ){
		float param = i / float(num_birds);
		float ang =  param * (2.f * PI);
		glm::vec2 point = 5.f * pointOnCicle( ang ); 
		float normalized = ang/(2.f * PI);
		float hang = normalized * 360.f;

		proto.setColor( protowizard::hsv2rgb( hang, 1,1 ) ); 
		proto.setTexture("assets/textures/whitehen.tga");
		proto.drawMesh( glm::vec3( point.x, 0.f, point.y), -90.f + hang, 0.f, "assets/models/googley_chicken.obj");
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

	glm::vec3 chicken_pos( 0.f );
	glm::vec3 chicken_dir( 0.f );
	float chicken_horiz_dir = 0.f;
	float chicken_verti_dir = 0.f;

	proto.setTitle( "your friendly example project" );
	proto.setFrameRate( 60 );
	do
	{
		proto.cls(0.5f, 0.5f, 0.5f);

		proto.setColor(0.f, 1.f, 0.f);
		
		proto.drawSphere( glm::vec3(-10.f, 0.f, 0.f), 1.0f );
		proto.drawCube( glm::vec3(-5.f, 0.f, 0.f) );
		proto.drawCone( glm::vec3( 0.f, -5.f, 0.f), glm::vec3(0.f, 5.f, 0.f), 0.5f );

		proto.setColor(1.f, 1.f, 1.f);
		proto.setTexture("assets/textures/lightcycle.png");
		proto.drawPlane( glm::vec3(5.0f, -1.f, 0.f), glm::vec3( 0.f, 0.f, 1.f ), 2.f );


		proto.setTexture("assets/textures/alpha_particle.png");
		proto.setLightBlend();
		proto.setAlpha( 0.125 );
		for( int i=0; i<100; i++ )
		{
			proto.drawPlane( glm::vec3(5.0f, 5.f, 0.f - i*0.05f), glm::vec3( 0.f, 0.f, 1.f ), 2.f );

			proto.drawPlane( glm::vec3(10.0f, 5.f + i*0.05f, -5.f ), glm::vec3( 0.f, 1.f, 0.f ), 2.f );
		}
		proto.setBlend( false );
		proto.setAlpha( 1.f );

		proto.setTexture("assets/textures/whitehen.tga");
		proto.setBlend( true );
		//proto.setLightBlend();
		for( int i=0; i<10; i++ )
		{
			proto.setColor( protowizard::hsv2rgb( i/10.f * 360.f, 1,1 ) ); 
			proto.drawMesh( glm::vec3(0.f + i*0.5f, -2.f, -1.f - i * 0.5f) , "assets/models/googley_chicken.obj");
		}
		proto.setBlend( false );

		float t1 = proto.klock();
		float t2 = t1 + 3.14f;
		float radius = 2.5f;
		proto.setColor(1.f, 0.f, 0.f);
		proto.drawCone( glm::vec3(10.f + cos(t1)* radius, sin(t1) * radius, 0.f), glm::vec3(10.f + cos(t2) * radius, sin(t2) * radius, 0.f), 0.5f );


		float normalized_mousx = proto.getMouseX() / (float)proto.getWindowWidth();
		float normalized_mousy = proto.getMouseY() / (float)proto.getWindowHeight();

		proto.setColor( 0.8f, 0.8f, 0.8f );

		chicken_horiz_dir = 360.f * normalized_mousx;
		chicken_verti_dir = 360.f * normalized_mousy;

		glm::mat4 identityMatrix(1.f);
		glm::mat4 xrot = glm::rotate( identityMatrix, chicken_verti_dir, glm::vec3(1.f, 0.f, 0.f) );
		glm::mat4 yrot = glm::rotate( identityMatrix, chicken_horiz_dir, glm::vec3(0.f, 1.f, 0.f) );
		glm::mat4 final = xrot * yrot;
		chicken_dir = ( glm::vec4(0.f, 0.f, 1.f, 0.f) * final ).xyz();

		float dir = float( proto.mouseDownLeft() - proto.mouseDownRight() );
		if ( fabs(dir) > 0.0f )
		{
			float units_per_sec = 5.f;
			float speed = units_per_sec * proto.getMSPF(); 

			chicken_pos += dir * speed * chicken_dir;
		}

		
		proto.setOrientation( glm::rotate( glm::transpose(final), 90.f, glm::vec3(0.f, 1.f, 0.f) ) );
		proto.setColor(1.f, 1.f, 1.f); 
		proto.setTexture("assets/textures/whitehen.tga");
		proto.drawMesh( chicken_pos, "assets/models/googley_chicken.obj");
		proto.setOrientation( identityMatrix );
		
		circle_of_birds( proto );

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