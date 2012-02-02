#ifndef _MATH_LINE_H_
#define _MATH_LINE_H_

#include <glm/glm.hpp>

namespace ProtoMath{

template <typename T>
struct line_t{
	T v0, v1;

	line_t( ) {

	}

	line_t(const T v0_, const T v1_ ) : v0(v0_), v1(v1_) 
	{
	}
};



// returns the point on the line ab that is closest to input point
glm::vec3 closestPtOnLine( const glm::vec3 &point, const glm::vec3 &a, const glm::vec3 &b )
{
   // from A to P
   glm::vec3 aTop = point - a;
   // From A to B
   glm::vec3 n = b - a;
   // normalize Dir A to B
   float distAB = glm::length(n);
   
   if( distAB > 0.0 ){
      n = n / distAB;
   }
   
   float t = glm::dot(n,aTop); // proj v1 onto norm
   
   if( t<= 0){ // A is closest
      return a;
   }
   
   if( t>= distAB){ // B is closest
      return b;
   }
   
   // create new vec, closest pt
   return a + n * t;
}

//void closest_pt_test(ProtoGraphics& proto, const glm::vec2 &norm_mouse){
//		vec3 p1 ( 0.0f, 0.0f, -20.0f );
//		vec3 p2 ( 10.0f, 0.0f, -20.0f );
//		vec3 pos ( -15.f + norm_mouse.x*30.f, -15.f + norm_mouse.y*30.f, -20.0f  );
//
//		const vec3 closest = closestPtOnLine( pos, p1, p2 );
//
//		proto.setColor(1.f, 1.f, 1.f);
//		proto.drawCone(p1,p2,0.125f);
//		proto.drawSphere(pos,0.5f);
//		proto.setColor(1.f, 0.f, 0.f);
//		proto.drawSphere(closest,0.25f);
//}

bool find_intersection(line_t<glm::vec2> &line1, line_t<glm::vec2> &line2, glm::vec2& hitpoint, float &s, float &t)
{
   glm::vec2 p1 = line1.v0;
   glm::vec2 p2 = line1.v1;
   glm::vec2 p3 = line2.v0;
   glm::vec2 p4 = line2.v1;

   // written by torb. 13. dec 2011
   float e = p1.x - p3.x;
   float f = p1.y - p3.y;
   
   float dx1 = p2.x - p1.x;
   float dy1 = p2.y - p1.y;
   
   float dx2 = p4.x - p3.x;
   float dy2 = p4.y - p3.y;
   
   float a = -dx1; float b = dx2;
   float c = -dy1; float d = dy2;
   
   float det = a*d - b*c;
   // if det == 0, then there is no soln
   if ( det == 0.f ) return false;
   
   // http://en.wikipedia.org/wiki/Cramer's_rule#Explicit_formulas_for_small_systems
   s = (e*d - b*f) / det;
   t = (a*f - e*c) / det;
   
   hitpoint.x = p1.x + s * dx1;
   hitpoint.y = p1.y + s * dy1;
   
   bool within_s = s >= 0.f && s <= 1.f;
   bool within_t = t >= 0.f && t <= 1.f;
   return within_s && within_t;
}

bool find_intersection(line_t<glm::vec2> &line1, line_t<glm::vec2> &line2 )
{
	glm::vec2 hitpoint;
	float s, t;
	return find_intersection(line1, line2, hitpoint, s, t );
}

}; // end namespace


#endif