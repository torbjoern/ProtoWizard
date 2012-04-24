#include <proto/protographics.h>
#include <proto/math/math_common.h>

// last tested timings in mspf
// in release: 7-8  
// in debug  : 20-21

int main(int argc, const char* argv[])
{
	ProtoGraphicsPtr proto = ProtoGraphics::create();
	if ( !proto->init(600,600,argv) )
	{
		fprintf(stderr, "failed to init proto-> remembered to set default data dir???");
	}


	do
	{
		proto->cls(0,0,0);

		//proto->setCamera( glm::vec3(0.f, -30.f, 0.f), glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f) );
		proto->setCamera( glm::vec3(0.f, 30.f, 0.f), 0.f, -90.f ); // setcam(0,-30,0,0,pi/2);
		proto->setScale( 1.f );
		proto->setOrientation( identityMatrix );
	   int wid = 64;
	   int dep = 64;
	   for(int i=0; i<wid; i++)
	   {
	      for(int j=0; j<dep; j++)
	      {
	         float u = i/(float)wid;
	         float v = j/(float)dep;
	         float x = -25.f + u * 50.f;
	         float z = -25.f + v * 50.f;
	         proto->setColor( u , v, 0 );
	         glm::vec3 p1( x, 0.f, z );
	         glm::vec3 p2( x, 2.f, z );
	         proto->drawCone(p1,p2,.5f);
	         //drawsph( x, 0, z, .5 );
	      }
	   }

	   float mspf = proto->getMSPF();
	   char buf[256];
	   sprintf_s(buf, 256, "%.2f mspf", 1000.f * mspf);
	   proto->setTitle( buf );

		proto->frame();

	} while ( proto->isWindowOpen() );

	return 0;
}


