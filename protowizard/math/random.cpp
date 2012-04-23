#include "random.h"

#include "math_common.h"
#include "../../depends/noise/perlin.h"

// Krand function taken from Ken Silverman's Polydraw source
static long kholdrand = 1, snormstat = 0; 
__declspec(naked) static long krand ()
{
	_asm
	{
		mov eax, kholdrand
		imul eax, 214013*2
		add eax, 2531011*2
		shr eax, 1
		mov kholdrand, eax
		ret
	}
}

double protowizard::rnd()
{
	static const float oneover2_31 = 1.f/2147483648.f;
	return ((double)krand())*(double)oneover2_31;
}
// fast float random in interval -1,1
// source by RGBA: http://www.rgba.org/articles/sfrand/sfrand.htm
float protowizard::sfrand()
{
	unsigned int a=(rand()<<16)|rand();  //we use the bottom 23 bits of the int, so one
	//16 bit rand() won't cut it.
	a=(a&0x007fffff) | 0x40000000;  

	return( *((float*)&a) - 3.0f );
}

float protowizard::random( float range_begin, float range_end ) 
{
	float normalized = (sfrand()*0.5f) + 0.5f;
	return range_begin + normalized * (range_end-range_begin);
}

glm::vec3 protowizard::randPointOnSphere(float radius) {
  float z = random(-radius, radius);
  float radius_sq = radius * radius;
  float zsq = z * z;
  float theta = random(0.f, 2.f*M_PI);
  float x = sqrt(radius_sq - zsq) * cos(theta);
  float y = sqrt(radius_sq - zsq) * sin(theta);
  return glm::vec3(x,y,z);
}

glm::vec3 protowizard::randomUnitVector()
{
	return glm::normalize( glm::vec3(sfrand(),sfrand(),sfrand()) );
}

double protowizard::noise(double x)
{
	return noise1(x);
}

double protowizard::noise(double x, double y)
{
	double vec[] = {x,y};
	return noise2(vec);
}

double protowizard::noise(double x, double y, double z)
{
	double vec[] = {x,y,z};
	return noise3(vec);
}

double protowizard::octaves_of_noise(int octaves, double x, double y, double z){
	double value = 0.0;
	double vec[3];
	for(int i=0; i<octaves; i++){
		vec[0] = x*pow( 2, (double)i );
		vec[1] = y*pow( 2, (double)i );
		vec[2] = z*pow( 2, (double)i );
		value += noise3( vec );
	}
	return value;
}