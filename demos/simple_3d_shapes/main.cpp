#include <proto/protographics.h>
#include <proto/math/math_common.h>
#include <proto/color_utils.h>

#include <iostream>
#include <string>

void funkyLogo( protowizard::ProtoGraphics& proto )
{
	proto.setBlend( true );
	proto.setAlpha( 0.5f );

	proto.setScale( 4.f );
	glm::mat4 rotmat = glm::rotate( identityMatrix, 45.f, glm::vec3(0.f, 0.f, 1.f) );
	proto.setOrientation( rotmat );

	proto.setColor( 1.f, 0.f, .0f );
	float z = -2.5f;
	proto.drawCube( glm::vec3(rotmat * glm::vec4(-2.5f, -2.5f, z, 1.f)) );
	proto.drawCube( glm::vec3(rotmat * glm::vec4(-2.5f, +2.5f, z, 1.f)) );
	proto.drawCube( glm::vec3(rotmat * glm::vec4(+2.5f, +2.5f, z, 1.f)) );
	proto.drawCube( glm::vec3(rotmat * glm::vec4(+2.5f, -2.5f, z, 1.f)) );

	proto.setColor( 1.f, 1.f, 1.f );
	z = 2.5f;
	proto.drawCube( glm::vec3(rotmat * glm::vec4(-2.5f, -2.5f, z, 1.f)) );
	proto.drawCube( glm::vec3(rotmat * glm::vec4(-2.5f, +2.5f, z, 1.f)) );
	proto.drawCube( glm::vec3(rotmat * glm::vec4(+2.5f, +2.5f, z, 1.f)) );
	proto.drawCube( glm::vec3(rotmat * glm::vec4(+2.5f, -2.5f, z, 1.f)) );

	proto.setBlend( false );
	proto.setOrientation( identityMatrix );
}

void wave( protowizard::ProtoGraphics& proto )
{
	float time = (float) proto.klock();
	
	float scale = 3.f;
	proto.setScale(scale);
	const int sideBySide = 128;
    const float fSideBySide = float(sideBySide);
	for ( int i=0; i<sideBySide; i++ ) {
		for ( int j=0; j<sideBySide; j++ ) {
            const float u = -.5f + i/fSideBySide;
            const float v = -.5f + j/fSideBySide;
			const float freq = 6.f * 6.28f;
            const float distOrigin = sqrt(u*u + v*v);
			const float wave = cos(distOrigin * freq + time);
            const float x = scale * fSideBySide*u;
            const float y = scale * 1.5f * wave - 5.f;
			const float z = scale * fSideBySide*v;
            
			
			proto.setColor( protowizard::hsv2rgb( 180.f + wave*45.f, 1.f, 1.f ));
			proto.drawCube( glm::vec3(x,y,z) );
		}
	}
}

int main(int argc, const char* argv[])
{
	protowizard::ProtoGraphics proto;

	if( !proto.init(640,480) )
	{
		throw "failed to init proto. remembered to set default data dir???";
		return 1;
	}

	while( proto.isWindowOpen() )
	{
		proto.cls(0,0,0);

		if ( proto.mouseDownLeft() == false )
		{
			float ang = TWO_PI * proto.getNormalizedMouse().x;
			float ca = cos(ang); 
			float sa = sin(ang); 
			float radi = 15.f;

			proto.getCamera()->lookAt( glm::vec3(ca*radi, 0.f, sa*radi),  glm::vec3(0.f, 0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f) );
		} else {

			// Move cam up, back and right
			bool left = proto.keystatus(protowizard::KEY::LEFT);
			bool right = proto.keystatus(protowizard::KEY::RIGHT);
			bool back = proto.keystatus(protowizard::KEY::DOWN);
			bool forwards = proto.keystatus(protowizard::KEY::UP);
			proto.getCamera()->update( left, right, back, forwards, (float)proto.getMouseX(), (float)proto.getMouseY(), proto.mouseDownRight(), (float)proto.getMSPF() );
		}

		proto.debugNormals( proto.keystatus('N') );
		
		funkyLogo( proto );

		char buf[512];
		double mspf = proto.getMSPF();
		sprintf_s( buf, 512, "fps = %.1f,  mspf: %.1f", 1.0 / mspf, 1000.0 * mspf );
		proto.setTitle( std::string(buf) );

		wave( proto );
		
		proto.frame();
	}
}