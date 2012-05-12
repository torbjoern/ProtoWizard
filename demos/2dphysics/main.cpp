// http://www.reddit.com/r/programming/comments/fkat8/
// http://www.phailed.me/2011/02/polygonal-collision-detection/
// http://fixplz.blourp.com/blog/=phys
// http://clevrepellent.blogspot.com/2011/03/f-physics-engine-secret.html


#include <proto/protographics.h>
#include <proto/color_utils.h>
#include <proto/math/random.h>
#include <proto/math/math_common.h>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

#include "phys.h"
#include "global.h"


poly makePoly( vecRef pos, scalar radius, int segments )
{
	vertexList verts;
	for (int i=0; i<segments; i++) {
		scalar a = TWO_PI - i/scalar(segments) * TWO_PI; // ccw winding
		verts.push_back( pos + radius * vec::polar(a) );
	}
	return poly( verts );
}

bool test()
{
	glm::vec2 midScreen( 0.5f*proto->getWindowWidth(), 0.5f*proto->getWindowHeight() );
	poly box = makeBox( midScreen, 100, 100 );

	bool isIn = true;
	for ( int i=0; i<2; i++ ){
		isIn &= containsv( box, midScreen+glm::vec2( protowizard::sfrand()*4, protowizard::sfrand()*4 ) );
	}

	bool isIn2 = true;
	for ( int i=0; i<2; i++ ){
		isIn2 &= containsv( box, glm::vec2( protowizard::sfrand()*4, protowizard::sfrand()*4 ) );
	}
	bool allGood = isIn==true && isIn2==false;
	return allGood;

}

void draw_models()
{
	glm::vec2 midScreen( 0.5f*proto->getWindowWidth(), 0.5f*proto->getWindowHeight() );
	glm::vec2 mouse_coords( (float)proto->getMouseX(), (float)proto->getMouseY() );

	poly shape1 = makeBox( midScreen, 60, 80 );
	//poly shape1 = makePoly( midScreen, 50.f, 7 );
	poly shape2 = makePoly( mouse_coords, 25.f, 6 );
	
	vertexList numIsects = poly_poly(shape1, shape2);

	if ( numIsects.size() > 0 ) {
		proto->setColor(1.f, 0.f, 0.f); // no seperating axis

		for (auto it=numIsects.begin(); it!=numIsects.end(); ++it){
			auto v = *it;
			proto->drawCircle( v.x, v.y, 3 );
		}

	} else {
		proto->setColor( 0.f, 1.f, 0.f ); 
	}

	glm::vec2 mouseCoord( (float)proto->getMouseX(), (float)proto->getMouseY() );
	if ( containsv(shape1, mouseCoord) ) {
		proto->drawCircle( mouseCoord.x, mouseCoord.y, -3 );
	}

	shape1.draw( glm::vec2(0.f) );
	shape2.draw( glm::vec2(0.f) );
}


int main(int argc, const char* argv[])
{
	proto = ProtoGraphics::create();

	if (!proto->init(700,500,argv) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}

	//typedef std::function< (void)<void> > vfun;
	
	
	
	test();

	proto->setFrameRate( 60 );

	auto randshape = []() -> poly {
		scalar n = 4.f + protowizard::rnd()*4.f;
		poly s;
		for (int i=1; i<int(n+1); i++ ){
			const float radius = 30.f + float(protowizard::rnd()*20.f);
			s.verts.push_back( radius * vec::polar(-i/n*TWO_PI) );
		}
		s.calcAxes();
		return s;
	};

	space mySpace;
	mySpace.dt = 1.0f / 60.f;

	for ( int i=0; i<30; i++ ) {
		glm::vec2 pos( protowizard::rnd()*700, protowizard::rnd()*200 );
		body b( 1.f, randshape(), pos );
		mySpace.list.push_back( b );

	}
	body b( 1.f, makeBox(700,50), glm::vec2(350., 450.) );
	b.mass = infinity;
	mySpace.list.push_back( b );
	
	char title_buf[256];
	do
	{
		proto->cls(0.f, 0.f, 0.f);

		float mspf = proto->getMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, %3.0f", 1000.f * mspf, 1.f/mspf);
		proto->setTitle( title_buf );


		//draw_models();

		mySpace.draw();
		mySpace.update();

		proto->frame();

	} while( proto->isWindowOpen() );


}