
#include "protographics.h"
#include "math/math_common.h"
#include "math/math_ray.h"

#include <iostream>

void scene(ProtoGraphics &proto, const glm::vec3& cameraPosition, const glm::vec3& cameraFocus)
{
	// Move cam up, back and right
	proto.setCamera(  glm::vec3(-5.f, 5.f, -5.f),  glm::vec3(0.f, 0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f) );

	float time = (float)proto.klock();

	float zero_to_one = sin(time)*.5f+.5f;
	ProtoMath::Ray ray( glm::vec3(-2.f,0.f,0.f), glm::vec3(10.f * zero_to_one,0.f,0.f) );
	proto.setColor(1.f, 0.f, 0.f);
	proto.drawCone( ray.origin, ray.origin+ray.dir, 0.1f );

	int num_boxes = 6;
	float box_diameter = 1.f;
	float box_spacing = 1.5f;
	proto.setAlpha( 0.5f );
	proto.setBlend( true );
	for ( int i=0; i<num_boxes; i++)
	{
		
		float xp;
		// Create a num between 0,1
		xp = i / (float)num_boxes;
		xp *= (num_boxes * (box_diameter + box_spacing) );

		glm::vec3 box_pos( xp, 0.f, 0.f);
		glm::vec3 box_dim( box_diameter );
		glm::vec3 box_normal;
		float t = 0.f;
		bool hit = ray.intersectBox( ray, box_pos, box_dim, box_normal, t );

		if ( hit && t <= 1.f ) 
		{
			proto.setColor( 1.f, 1.f, 0.f );
		}else{
			proto.setColor( 1.f, 1.f, 1.f );
		}

		proto.setScale( box_dim.x );
		proto.drawCube( box_pos );
		proto.setScale( 1.f );
	}
	proto.setBlend( false );
	proto.setAlpha( 1.0f );

}


int main()
{
	using glm::vec3;

	ProtoGraphics proto;
	if (proto.init(320,240) == false )
	{
		std::cerr << "proto failed to init" << std::endl;
		return 1;
	}

	
	glm::vec3 cameraPosition( 0.f, 0.f, -5.f );
	glm::vec3 cameraFocus( 0.f, 0.f, 0.f );
	do
	{
		proto.cls(0,0,0);

		scene(proto, cameraPosition, cameraFocus );

		proto.frame();
	}while( proto.isWindowOpen() );
	return 0;
}