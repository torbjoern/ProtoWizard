#include <iostream>

#include <protographics.h>


class ProtoApp
{
public:
	ProtoApp()
	{
	}

	ProtoApp(const char* argv[])
	{
		if ( !proto.init(640,480,argv) )
		{
			std::cerr << "proto failed to init. either because of missing shaders, bad shaders.. or bad opengl drivers. or just bad code in proto" << std::endl;
			return;
		}

		xres = (float)proto.getWindowWidth();
		yres = (float)proto.getWindowHeight();

	}

	virtual void run()
	{
		do
		{
			proto.cls(0,0,0);
			proto.drawCircle( xres/2, yres/2, 5 );
			proto.frame();
		}while( proto.isWindowOpen() );
	};

	float xres, yres;

	ProtoGraphics proto;
};