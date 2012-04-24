#include <protographics.h>
#include "color_utils.h"
#include "math/random.h"

#include <memory>

namespace {
	ProtoGraphicsPtr proto;
}

void do_fractal()
{
	// Attractor position
	double x,y,z;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	// Attractor coefficients
	double a,b,c,d,e,f;

	//a = 1.4, b = -2.3, c = 2.4, d = -2.1;
	a = 1.78125, b = -0.78125, c = 1.90625, d = 2.65625, e = 0.7, f = -1.1;

	float xres = (float)proto->getWindowWidth();
	float yres = (float)proto->getWindowHeight();

	int itersMax = 1000;
	for(int i=0; i<itersMax; i++){

		double la = a; 
		double lb = b; 
		double lc = c; 
		double ld = d;
		//Add Noise to Coefficients for Smoothness
		{
			la += (double)protowizard::random(-0.001f, 0.001f);  lb += (double)protowizard::random(-0.001f, 0.001f);
			lc += (double)protowizard::random(-0.001f, 0.001f);  ld += (double)protowizard::random(-0.001f, 0.001f);
			//le += proto->random(-0.001, 0.001);  lf += proto->random(-0.001, 0.001);
		}

		// calc next position
		double xn = sin( la*y ) - cos( lb*x );
		double yn = sin( lc*x ) - cos( ld*y );
		double zn = sin( e*x ) - cos( f*z );
		// update
		x = xn;
		y = yn;
		z = zn;

		float scale = 100.f;

		float plotx = 0.5f * xres + (float)x*scale;
		float ploty = 0.5f * yres - (float)y*scale;
   
		float red = (float)z * 0.9f + (1.0f-(float)z) * 0.6f;
		float gre = (float)z * 0.2f + (1.0f-(float)z) * 0.4f;
		float blu = (float)z * 0.5f + (1.0f-(float)z) * 0.9f;
		proto->setColor( red, gre, blu );

		proto->drawCircle( plotx, ploty, 0.5f );
		//proto->drawSphere( glm::vec3(x,y,z)*6.5f, 0.05f );
	}
}


int main(int argc, const char* argv[])
{
	proto = ProtoGraphics::create();

	if (!proto->init(640,480,argv) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}


	//proto->setFrameRate( 60 );
	proto->cls(0.f, 0.f, 0.f);
	
	char title_buf[256];
	do
	{
		float mspf = proto->getAverageMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, %3.0f", 1000.f * mspf, 1.f/mspf);
		proto->setTitle( title_buf );

		if ( proto->keyhit(' ') )
		{
			fprintf(stderr, "you pressed space\n");
			proto->cls(0,0,0);
		}

		float normalized_mx = proto->getMouseX() / (float)proto->getWindowWidth();
		float normalized_my = proto->getMouseY() / (float)proto->getWindowHeight();
		glm::vec2 normalized_mouse(normalized_mx, normalized_my);

		proto->setAlpha( 0.05f );

		double timeStart = proto->klock();
		
		//proto->setBlend(true);
		proto->setLightBlend();
		do_fractal();
		
		double tot = proto->klock() - timeStart;
		printf("%f ms\n", 1000.0 * tot);

		proto->setBlend(false);
		proto->setColor(1,1,1);

		//float ang = normalized_mouse.x*6.28f;
		//glm::vec3 cam_pos( cos(ang), 0.f, sin(ang) );
		//cam_pos *= 25.f;
		//proto->setCamera( cam_pos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f) );

		proto->frame();

	} while( proto->isWindowOpen() );
}