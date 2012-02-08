#include <protographics.h>
#include "color_utils.h"



void do_fractal( ProtoGraphics &proto )
{
	// Attractor position
	float x,y,z;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	// Attractor coefficients
	float a,b,c,d,e,f;

	//a = 1.4;
	//b = -2.3;
	//c = 2.4;
	//d = -2.1;

	a = 1.78125, b = -0.78125, c = 1.90625, d = 2.65625, e = 0.7, f = -1.1;

	float xres = (float)proto.getWindowWidth();
	float yres = (float)proto.getWindowHeight();

	int itersMax = 1e4;
	for(int i=0; i<itersMax; i++){

		float la = a; 
		float lb = b; 
		float lc = c; 
		float ld = d;
		//Add Noise to Coefficients for Smoothness
		{
			la += proto.random(-0.001, 0.001);  lb += proto.random(-0.001, 0.001);
			lc += proto.random(-0.001, 0.001);  ld += proto.random(-0.001, 0.001);
			//le += proto.random(-0.001, 0.001);  lf += proto.random(-0.001, 0.001);
		}

		// calc next position
		float xn = sin( la*y ) - cos( lb*x );
		float yn = sin( lc*x ) - cos( ld*y );
		float zn = sin( e*x ) - cos( f*z );
		// update
		x = xn;
		y = yn;
		z = zn;

		float scale = 100.f;

		float plotx = 0.5f * xres + x*scale;
		float ploty = 0.5f * yres - y*scale;
   
		float red = z * 0.9 + (1.0f-z) * 0.6f;
		float gre = z * 0.2 + (1.0f-z) * 0.4f;
		float blu = z * 0.5 + (1.0f-z) * 0.9f;
		proto.setColor( red, gre, blu );

		proto.drawCircle( plotx, ploty, 0.5f );
		//proto.drawSphere( glm::vec3(x,y,z)*6.5f, 0.05f );
	}
}


int main()
{
	ProtoGraphics proto;

	if (!proto.init(640,480) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}


	proto.setFrameRate( 60 );
	proto.cls(0.f, 0.f, 0.f);
	
	char title_buf[256];
	do
	{
		float mspf = proto.getMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, %3.0f", 1000.f * mspf, 1.f/mspf);
		proto.setTitle( title_buf );

		if ( proto.keyhit(' ') )
		{
			fprintf(stderr, "you pressed space\n");
			proto.cls(0,0,0);
		}

		float normalized_mx = proto.getMouseX() / (float)proto.getWindowWidth();
		float normalized_my = proto.getMouseY() / (float)proto.getWindowHeight();
		glm::vec2 normalized_mouse(normalized_mx, normalized_my);

		proto.setAlpha( 0.05f );

		double timeStart = proto.klock();
		
		//proto.setBlend(true);
		proto.setLightBlend();
		do_fractal(proto);
		
		double tot = proto.klock() - timeStart;
		printf("%f ms\n", 1000.0 * tot);

		proto.setBlend(false);
		proto.setColor(1,1,1);

		//float ang = normalized_mouse.x*6.28f;
		//glm::vec3 cam_pos( cos(ang), 0.f, sin(ang) );
		//cam_pos *= 25.f;
		//proto.setCamera( cam_pos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f) );

		proto.frame();

	} while( proto.isWindowOpen() );


}