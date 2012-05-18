#include <proto/protographics.h>
#include <proto/math/math_common.h>
#include <proto/color_utils.h>
#include <proto/math/random.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>


namespace {
	protowizard::ProtoGraphicsPtr proto;
}

glm::vec3 random_vector() {
	return glm::vec3( protowizard::random( -1.f, +1.f ),protowizard::random( -1.f, +1.f ), protowizard::random(-1.f, +1.f) );
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
		size = protowizard::random( .1f, 1.f );
		
		// ROY G. BIV
		// red orange yellow green blue indigo violet
		float interval = 360.f / 7.f;

		const float colors[] = {interval*0, interval*1, interval*2, interval*3, interval*4, interval*5, interval*6 };
		float green_hue = protowizard::random( colors[KOLORS::green], colors[KOLORS::blue] );
		color = protowizard::hsv2rgb( green_hue, 1.f, 1.f );

		life = protowizard::random( 0.5f, max_life );
	}

	void update(float timestep) {
		pos += 15.f * dir * timestep;
		life -= timestep;
	};

	void draw() {
		proto->setColor( color );
		proto->setAlpha( life / max_life );
		//proto->setAlpha(1);
		proto->setTexture(proto->getResourceDir() + "/textures/alpha_dot.png");
		proto->drawPlane( pos, proto->getCamera()->getLookDirection(), 0.5f );
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



struct Magnetic
{
	const static int NUMP = 1024;
	glm::vec3* particlePos;

	const static int MAXPOLES = 128;
	glm::vec3 polePos[MAXPOLES];
	float polarity[MAXPOLES];
	int positivePole[MAXPOLES];
	int positiveCount;
	
	float worldSiz;
	float poleRadi;
	float dt;

	Magnetic() {
		worldSiz = 15.f;
		poleRadi = 0.1f;
		dt = 1.f / 60.f;
		particlePos = new glm::vec3[NUMP];
	}

	~Magnetic() {
		delete[] particlePos;
	}

	void init() {
		 for(int i=0;i<NUMP;i++){
			 particlePos[i] = glm::vec3( protowizard::random(-worldSiz,worldSiz), 
				                         protowizard::random(-worldSiz,worldSiz), 
										 protowizard::random(-worldSiz,worldSiz)
									   );
		 }
		 //initPolesSphere();
		 initPolesBox();
	}

	glm::vec3 SphericalToCartesian(float polar, float elevation, float radius){
		float a = radius * cos(elevation);
		return glm::vec3( a * cos(polar), radius * sin(elevation), a * sin(polar) );
	}

	void countPositive() {
		 // save poles with +charge
		 positiveCount = 0;
		 for(int i=0; i<MAXPOLES; i++)
		 {
			if(polarity[i] == +1.0)
			{
			   positivePole[positiveCount] = i;
			   positiveCount++;
			}
		 }
	}

	void initPolesBox()
	{
		// place poles randomly in space
		float dist = worldSiz * 0.5f;
		 for(int i=0; i<MAXPOLES; i++){
			polePos[i] = glm::vec3( protowizard::random(-dist,dist), 
				                    protowizard::random(-dist,dist), 
									protowizard::random(-dist,dist)
								  );
			if(  protowizard::random(-1.f, 1.f) > 0.f ) {
				polarity[i] = +1.0;
			} else {
				polarity[i] = -1.0;
			}
		 }
     
		countPositive();
	}

	void initPolesSphere(){
		float hang = 0;
		float vang = 0;
		float squareRoot = sqrt( float(MAXPOLES) );
		float hangInc = TWO_PI / squareRoot;
		float vangInc = TWO_PI / squareRoot;
		for(int i=0; i<MAXPOLES; i++){
			//polePos[i] = protowizard::randPointOnSphere(worldSiz*0.75f);
			polePos[i] = SphericalToCartesian( hang, vang, worldSiz*0.5f );
			//polarity[i] = -1.0;
			hang += hangInc;
			if ( hang >= TWO_PI ) {
				hang = 0;
				vang += vangInc;
			}
			if ( polePos[i].y < 0.f ) {
				polarity[i] = +1.0;
			} else {
				polarity[i] = -1.0;
			}
			//if(  protowizard::random(-1.f, 1.f) > 0.f ) {
			//	polarity[i] = +1.0;
			//} else {
			//	polarity[i] = -1.0;
			//}
		}
		countPositive();
		 
		
	}

	void update() { 
		//static glm::vec3 targ(0.f);

		//if ( glm::distance(targ, polePos[0] ) < poleRadi ) {
		//	float boxHalf = 5.f;
		//	targ = glm::vec3( protowizard::random(-boxHalf,boxHalf), 
		//		                         protowizard::random(-boxHalf,boxHalf), 
		//								 protowizard::random(-boxHalf,boxHalf)
		//							   );
		//}
		//glm::vec3 toTarg = targ - polePos[0];
		//polePos[0] += 10.f * dt * glm::normalize(toTarg);
		//polePos[0] = proto->noise( polePos[0].x, polePos[0].y, polePos[0].z );

		proto->setLightBlend();
		//proto->setAlpha(0.75f);
		proto->setAlpha(1.f);
		for(int i=0;i<NUMP;i++)
		{
			float x = particlePos[i].x; 
			float y = particlePos[i].y;
			float z = particlePos[i].z;
     
			bool dead = false;
			if( x < -worldSiz || x > worldSiz ) dead = 1;
			if( y < -worldSiz || y > worldSiz ) dead = 1;
			if( z < -worldSiz || z > worldSiz ) dead = 1;
     
			glm::vec3 vel(0.f);
     
			if(dead==false)
			{
			for(int j=0; j<MAXPOLES; j++)
			{
				glm::vec3 fromPole = particlePos[i] - polePos[j];
				float distSq = glm::dot( fromPole, fromPole );
           
				// kill when hit sink...
				if( polarity[j] < 0 && distSq < poleRadi){
					dead = true;
					break;
				} 
       
				float nd = 1.0f/distSq; // normalize
				vel += fromPole*nd*polarity[j];
			}      
			vel /= (float)MAXPOLES;
			}

			float mag = glm::length( vel );
			// respawn if dead or moving too slow
			if ( dead || mag < 1e-4f || mag==0.f ) {
				int randPole = positivePole[int(protowizard::rnd()*positiveCount)]; // Choose a positive pole to spawn on.
				glm::vec3 randP = protowizard::randPointOnSphere( poleRadi );
				particlePos[i] = polePos[randPole] + randP;
			} else {       
				vel = vel / mag; // normalize. looks better

				//float red = (i%72)*64.f+64.f;
				//float green = (i%80)+128.f;
				//float blue = 64.f;
				//float r255 = 1.f/255.f;
				//proto->setColor( red*r255, green*r255, blue*r255 );
				proto->setColor( (i%63) / 255.f, (i%23) / 255.f, .125f );
				float len = .25f;// / mag;
				//glm::vec3 dir = glm::normalize(vel) * len;
				proto->drawSphere( particlePos[i], 0.1f );
				//proto->drawCone( pos-dir, pos+dir, 0.05f );
				//proto->setTexture(proto->getResourceDir() + "/textures/alpha_dot.png");
				//proto->drawPlane( pos, proto->getCamera()->getLookDirection(), 0.1f );

				// Move
				float f = 10*dt;
				particlePos[i] += vel * f;
			}
		}

			proto->setBlend(false);
			proto->disableTexture();
			for(int i=0; i<MAXPOLES; i++){
				if ( polarity[i] == +1.0 ) proto->setColor(1.f, 0.f, 0.f); else proto->setColor(0.f, 0.f, 1.f);
				proto->drawSphere( polePos[i], poleRadi );
			}
		 } // end method
};

void texturedTest(){
	proto->setTexture(proto->getResourceDir() + "/textures/alpha_dot.png");
	
	//proto->disableLight();
	proto->setBlend(true);
	//proto->setLightBlend();
	proto->setAlpha(0.5f);
	glm::vec3 pos(0.f);
	proto->setColor(1.f,1.f,0.f); proto->drawPlane( pos + glm::vec3(-10.f, 0.f, 0.f), proto->getCamera()->getLookDirection(), 2.5f );
	proto->setColor(1.f,1.f,1.f); proto->drawPlane( pos + glm::vec3(-5.f, 0.f, 0.f), proto->getCamera()->getLookDirection(), 2.5f );
	proto->setColor(1.f,0,0); proto->drawPlane( pos + glm::vec3(0.f, 0.f, 0.f),proto->getCamera()->getLookDirection(), 2.5f );
	proto->setColor(0.f,1.f,0.f); proto->drawPlane( pos + glm::vec3(+5.f, 0.f, 0.f), proto->getCamera()->getLookDirection(), 2.5f );
	proto->setColor(0.f,0.f,1.f); proto->drawPlane( pos + glm::vec3(+10.f, 0.f, 0.f), proto->getCamera()->getLookDirection(), 2.5f );

	proto->disableTexture();
	proto->setBlend(false);
	proto->setColor(1.f,1.f,1.f);
	proto->setScale( 10.f, 5.f, 1.f );
	proto->drawCube( glm::vec3(0.f, 0.f, -5.f) );
	proto->setScale( 1.f );
}

void testRandomPointOnSphere()
{
	proto->setColor( 1.f, 1.f, 1.f );

	for (int i=0; i<100; i++){
		float radii = 5.f;
		glm::vec3 p = protowizard::randPointOnSphere( radii );
		float dist = glm::length(p);
		if ( dist < (radii-1e-6f) || dist > (radii+1e-6f) ) {
			throw std::runtime_error("perhaps some error margin");
		}
		proto->drawSphere( p, 0.1f );
	}

}

void enableFloatExceptions()
{
	//Set the x86 floating-point control word according to what
	//exceptions you want to trap.
	_clearfp(); //Always call _clearfp before setting the control
	//word
	//Because the second parameter in the following call is 0, it
	//only returns the floating-point control word
	unsigned int cw = _controlfp(0, 0); //Get the default control
	//word
	//Set the exception masks off for exceptions that you want to
	//trap. When a mask bit is set, the corresponding floating-point
	//exception is //blocked from being generating.
	cw &=~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL|EM_INVALID);
	//For any bit in the second parameter (mask) that is 1, the
	//corresponding bit in the first parameter is used to update
	//the control word.
	unsigned int cwOriginal = _controlfp(cw, MCW_EM); //Set it.
	//MCW_EM is defined in float.h.
	//Restore the original value when done:
	//_controlfp(cwOriginal, MCW_EM);
}

int globalVar;
int EvaluateException(int exceptionCode)
{
	bool isOverflow = exceptionCode == EM_OVERFLOW;
	bool isUnderflow = exceptionCode == EM_UNDERFLOW;
	bool isZeroDiv = exceptionCode == EM_ZERODIVIDE;
	bool isDenorm = exceptionCode == EM_DENORMAL;
	bool isInvalid = exceptionCode == EM_INVALID;
	//May evaluate exeptionCode
	//May set a global variable that can be evaluated in
	//or after _except guard code.
	globalVar = _clear87(); //Get floating-point status word.
	//The above function also sets the
	//busy bit to 0 to show that FPU
	//is not busy.
	//return EXCEPTION_EXECUTE_HANDLER;//force execution of
	//the _except() guard code.
	return 0;
}

void testFP()
{
	float zero = 0.f;
	float err = 1.f / zero;
}

int main(int argc, const char* argv[])
{
	//enableFloatExceptions();
	//testFP();

	proto = protowizard::ProtoGraphics::create();
	if( !proto->init(1024,768) )
	{
		std::cerr << "failed to init" << std::endl;
		return -1;
	}
	

	ParticleSystem particleSystem;
	Magnetic magneticEffect;
	magneticEffect.init();
	proto->setFrameRate(60);
	//proto->debugNormals( true );

	while( proto->isWindowOpen() )
	{
		//proto->cls(0,0,0);
		proto->clz();

		if ( proto->mouseDownLeft() == false )
		{
			float ang = TWO_PI * proto->getNormalizedMouseX();
			float ca = cos(ang); 
			float sa = sin(ang); 
			float radi = 20.f;

			proto->getCamera()->lookAt(  glm::vec3(ca*radi, 0.f, sa*radi),  glm::vec3(0.f, 0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f) );
		} else {
			// Move cam up, back and right
			proto->getCamera()->lookAt(  glm::vec3(-10.f, 10.f, -10.f),  glm::vec3(0.f, 0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f) );
		}



		float interval = 360.f / 7.f;
		const float colors[] = {interval*0, interval*1, interval*2, interval*3, interval*4, interval*5, interval*6 };
		float green_hue = protowizard::random( colors[KOLORS::green], colors[KOLORS::blue] );
		glm::vec3 color = protowizard::hsv2rgb( green_hue, 1.f, 1.f );
		particleSystem.create( 8, color ); // 60 fps. create Nx60 particles pr sec

		if ( proto->keyhit(' ') ) {
			float hue = protowizard::random( colors[KOLORS::red], colors[KOLORS::yellow] );
			glm::vec3 color = protowizard::hsv2rgb( hue, 1.f, 1.f );
			particleSystem.create( 150, color );
		}

		//particleSystem.update();

		
		proto->disableTexture();
		proto->setColor( 1.f, 0.f, 0.f );
		proto->setBlend( false );
		auto billBoard = glm::mat4( 
				glm::vec4(1.f, 0.f, 0.f, 0.f),
				glm::vec4(0.f, 4.f, 0.f, 0.f),
				glm::vec4(0.f, 0.f, 1.f, 0.f),
				glm::vec4(0.f, 0.f, 0.f, 1.f)
				);
		//proto->setOrientation( billBoard );
		//proto->drawPlane( glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), 1.f );

		//proto->setLightBlend();
		//particleSystem.draw();

		if ( proto->keyhit('R') ) magneticEffect.init();
		magneticEffect.dt = proto->getMSPF();
		proto->setOrientation( identityMatrix );
		magneticEffect.update();

		// Blend in a black quad
		if (1)
		{
			proto->setColor(0.f, 0.f, 0.f);
			proto->setBlend(true);
			proto->setAlpha( 1.f / 64.f );
			glm::vec3 camNorm = proto->getCamera()->getLookDirection();
			proto->drawPlane( proto->getCamera()->getPos() + camNorm, camNorm, 1.5f );
		}

		proto->frame();
		static char title_buf[256];
		float mspf = proto->getAverageMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, parts %i", 1000.f * mspf, particleSystem.num_alive);
		proto->setTitle( title_buf );
	}
}