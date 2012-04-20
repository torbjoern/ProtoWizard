#include <protographics.h>
#include <math/math_common.h>
#include <color_utils.h>

#include <iostream>
#include <string>
#include <algorithm>

namespace {
	ProtoGraphics proto;
}

glm::vec3 random_vector() {
	return glm::vec3( proto.random( -1.f, +1.f ), proto.random( -1.f, +1.f ), proto.random(-1.f, +1.f) );
}

namespace KOLORS {
	enum eColor { red, orange, yellow, green, blue, indigo, violet };
}


struct Particle
{
	glm::vec3 color;
	glm::vec3 pos;
	glm::vec3 dir;
	float size;
	float life;
	float max_life;

	Particle () {
		max_life = 2.f;
		life = 0;
	}

	void init(){
		pos = glm::vec3(0.f);
		dir = glm::normalize( glm::vec3( random_vector() ) );
		size = proto.random( .1f, 1.f );
		
		// ROY G. BIV
		// red orange yellow green blue indigo violet
		float interval = 360.f / 7.f;

		const float colors[] = {interval*0, interval*1, interval*2, interval*3, interval*4, interval*5, interval*6 };
		float green_hue = proto.random( colors[KOLORS::green], colors[KOLORS::blue] );
		color = protowizard::hsv2rgb( green_hue, 1.f, 1.f );

		life = proto.random( 0.5f, max_life );
	}

	void update(float timestep) {
		pos += 15.f * dir * timestep;
		life -= timestep;
	};

	void draw() {
		proto.setColor( color );
		proto.setAlpha( life / max_life );
		//proto.setAlpha(1);
		proto.setTexture(proto.getResourceDir() + "/textures/alpha_dot.png");
		proto.drawPlane( pos, proto.getCamera()->getLookDirection(), 3.f );
	};
};

struct ParticleSystem
{
	std::vector<Particle> particles;
	int num_alive;

	ParticleSystem() {
		particles.resize( 1000 );
		num_alive = 0;
	}

	void create( int num, const glm::vec3& color ) {
 		int num_visited = 0;

		for( size_t i=0; i<particles.size(); i++ ) {
			Particle& p = particles[i];
			if ( p.life <= 0.f ) {
				p.init();
				p.color = color;
				num_visited++;
			}
			if ( num_visited == num ) break;
		};

		//std::for_each( particles.begin(), particles.end(), [&](Particle& p) {
		//	if ( p.life <= 0.f ) {
		//		p.init();
		//		num_visited++;
		//	}
		//});
	}

	void update() {
		num_alive = 0;
		std::for_each( particles.begin(), particles.end(), [&](Particle& p) {
			if( p.life > 0 ) {
				p.update(1.f / 60.f);
				num_alive++;
			}
		});
	}

	void draw() {
		std::for_each( particles.begin(), particles.end(), [](Particle& p) {
			if( p.life > 0 ) p.draw();
		});
	}
};

void drawArm( float angle )
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
	if( !proto.init(640,480,argv) )
	{
		std::cerr << "failed to init" << std::endl;
		return -1;
	}

	//proto.debugNormals( true );
	ParticleSystem particleSystem;
	proto.setFrameRate(60);

	float t = 0.f;
	while( proto.isWindowOpen() )
	{
		proto.cls(0,0,0);

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



		float interval = 360.f / 7.f;
		const float colors[] = {interval*0, interval*1, interval*2, interval*3, interval*4, interval*5, interval*6 };
		float green_hue = proto.random( colors[KOLORS::green], colors[KOLORS::blue] );
		glm::vec3 color = protowizard::hsv2rgb( green_hue, 1.f, 1.f );
		particleSystem.create( 8, color ); // 60 fps. create Nx60 particles pr sec

		if ( proto.keyhit(' ') ) {
			float hue = proto.random( colors[KOLORS::red], colors[KOLORS::yellow] );
			glm::vec3 color = protowizard::hsv2rgb( hue, 1.f, 1.f );
			particleSystem.create( 150, color );
		}

		particleSystem.update();

		proto.setLightBlend();
		
		particleSystem.draw();

		proto.frame();
		static char title_buf[256];
		float mspf = proto.getAverageMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, parts %i", 1000.f * mspf, particleSystem.num_alive);
		proto.setTitle( title_buf );

		t += 1.f / 500.f;
	}
}