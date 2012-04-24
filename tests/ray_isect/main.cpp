
#include <proto/protographics.h>
#include <proto/math/math_common.h>
#include <proto/math/math_ray.h>

#include <iostream>

ProtoGraphicsPtr proto;

void scene()
{
	if ( proto->mouseDownLeft() )
	{
		float ang = TWO_PI * proto->getMouseX() / (float)proto->getWindowWidth();
		float ca = cos(ang); 
		float sa = sin(ang); 
		float radi = 15.f;

		proto->setCamera(  glm::vec3(ca*radi, 0.f, sa*radi),  glm::vec3(0.f, 0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f) );
	} else {
		// Move cam up, back and right
		proto->setCamera(  glm::vec3(-10.f, 10.f, -10.f),  glm::vec3(0.f, 0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f) );
	}

	float time = (float)proto->klock();

	float zero_to_one = sin(time)*.5f+.5f;
	glm::vec3 rayPos = glm::vec3(-5.f, 4.f*cos(time*0.25f), 4.f*sin(time*0.25f) );
	protomath::Ray ray( rayPos, glm::vec3(10.f + 10.f * zero_to_one,0.f,0.f) );
	proto->setColor(1.f, 0.f, 0.f);
	proto->drawCone( ray.origin, ray.origin+ray.dir, 0.1f );

	int num_boxes = 6;
	float box_diameter = 2.f;
	float box_spacing = 0.25f;

	for ( int i=0; i<2; i++)
	for ( int j=0; j<num_boxes; j++)
	for ( int k=0; k<num_boxes; k++)
	{
		const float u = i / (float)num_boxes;
		const float v = j / (float)num_boxes;
		const float w = k / (float)num_boxes;
		const float dist = 0.5f * num_boxes * 2.f*(box_diameter + box_spacing );
		const float xp = u*dist;
		const float yp = -0.5f * dist + v*dist;
		const float zp = -0.5f * dist + w*dist;
		

		glm::vec3 box_pos( xp, yp, zp);
		glm::vec3 box_dim( box_diameter );
		glm::vec3 box_normal;
		float t = 0.f;
		bool hit = ray.intersectBox( ray, box_pos, box_dim, box_normal, t );

		if ( hit )  //  && t <= 1.f
		{
			proto->setColor( 0.f, 0.f, 1.f );
			glm::vec3 hit_point = ray.origin + ray.dir * t;
			proto->drawSphere( hit_point, 0.3f );
			
			proto->setColor( 1.f, 1.f, 0.f );
			proto->setScale( box_dim.x ); proto->drawCube( box_pos + box_dim * 0.5f ); proto->setScale( 1.f );
		}else{
			proto->setColor( 1.f, 1.f, 1.f );

			proto->setScale( box_dim.x ); 
			proto->setAlpha( 0.5f ); proto->setBlend( true ); proto->drawCube( box_pos + box_dim * 0.5f ); proto->setBlend( false );
			proto->setScale( 1.f );
		}
	}


}


int main(int argc, const char* argv[])
{
	using glm::vec3;
	proto = ProtoGraphics::create();
	if (proto->init(640,480,argv) == false )
	{
		std::cerr << "proto failed to init" << std::endl;
		return 1;
	}

	do
	{
		proto->cls(0,0,0);

		scene();

		proto->frame();
	}while( proto->isWindowOpen() );
	return 0;
}