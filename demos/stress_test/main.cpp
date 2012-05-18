#include <proto/protographics.h>
#include <proto/math/math_common.h>

// last tested timings in mspf
// num: 64x64 = 4096
// in release: 7-8  
// in debug  : 20-21

int main(int argc, const char* argv[])
{
	protowizard::ProtoGraphicsPtr proto = protowizard::ProtoGraphics::create();
	if ( !proto->init(600,600) )
	{
		throw "failed to init proto-> remembered to set default data dir???";
		return 1;
	}

	do
	{
		proto->cls(0,0,0);

		//proto->setCamera( glm::vec3(0.f, -30.f, 0.f), glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f) );
		proto->getCamera()->set( glm::vec3(0.f, 30.f, 0.f), 0.f, -90.f ); // setcam(0,-30,0,0,pi/2);
		proto->setScale( 1.f );
		proto->setOrientation( identityMatrix );
	   int wid = 40; // 64 40
	   int dep = 40; // 64 40
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
	         //proto->drawCone(p1,p2,.5f);
			 proto->drawPlane( p1, glm::vec3(0.f, 1.f, 0.f), 0.5f );
			 //proto->drawCube( p1 );
	         //proto->drawSphere( p1, .5f );
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


