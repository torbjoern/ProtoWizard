#define GLM_SWIZZLE 
#include <glm/glm.hpp>

#include "protographics.h"
#include "math/math_line.h"
#include "math/math_common.h"

#include <functional>
#include <cassert>
#include <iostream>

#define for_each(_ITER_, _COLL_) for (auto _ITER_ = begin(_COLL_); _ITER_ != end(_COLL_); ++_ITER_)

using glm::vec3;
typedef float real;

const vec3 zero_vec( (real) 0.0 );
vec3 gravity_dir( 0.0f, -1.0f, 0.0f );


float RAD = 1.05f; // 1.0
float CYL_RAD = 0.5f; // 0.5

const int PHYSHZ = 256;


struct ray_t{
	glm::vec3 origin;
	glm::vec3 dir;

	float distanceSquared( const glm::vec3 &point  )
	{
		glm::vec3 w = point - origin;
		float proj = glm::dot(w, dir);
		if ( proj <= 0.f ){
			return glm::dot( w,w );
		}else{
			float vsq = glm::dot( dir, dir );
			if ( proj >= vsq ) {
				return glm::dot( w,w ) - 2.0f*proj + vsq;
			}else{
				return glm::dot( w,w ) - proj*proj/vsq;
			}
		}

		return 0.f;
	}
};

void test_ray_point_square( ProtoGraphics& proto, const glm::vec2 &norm_mouse ){
	ray_t ray;
	ray.origin = glm::vec3( 0.f );

	float a = norm_mouse.x * 6.28f;
	ray.dir = 10.f * glm::vec3( cos(a), 0.f, sin(a) );

	glm::vec3 p( 5.f, 0.f, 5.f );


}

struct cyl_t{
	glm::vec3 e1, e2;
	float radi;
};

bool isect_ray_cyl( ray_t ray, cyl_t cyl, float& t )
{
   glm::vec3 s0 = ray.origin - cyl.e1;
   glm::vec3 e2e1 = cyl.e2 - cyl.e1;
   float e2e1Sq = glm::dot(e2e1,e2e1);
   float vDotV = glm::dot(ray.dir, ray.dir);   
   
   float dirDotAxis = glm::dot(ray.dir, e2e1);   
   
   float s0DotAxis = glm::dot(s0, e2e1);

   float a = vDotV - ( dirDotAxis*dirDotAxis / e2e1Sq );
   float b = glm::dot(s0, ray.dir) - ( s0DotAxis * dirDotAxis) / e2e1Sq;
   float c = glm::dot(s0,s0) - cyl.radi*cyl.radi - (s0DotAxis*s0DotAxis) / e2e1Sq;
   
   float D4 = b*b - a*c;
   if ( D4 > 0.f )
   {

      t = (-b - sqrt(b*b - a*c)) / a;
      
      if ( t < 0 || t > 1.0 )
      {
         return false;
      }

	  glm::vec3 pt = ray.origin + t * ray.dir;
      
      // The signed length L of the projection P(t) - E1 onto A
      float L = glm::dot( pt, e2e1 ) / glm::length(e2e1) ;
      
      // The ray isects the cyl if L is positive and less than ||A||
      // we simply need to check that
      // 0 < [P(t) - E1] dot A < A^2
      
      glm::vec3 e1pt = pt - cyl.e1;
      float proj_pt_e1 = glm::dot( e1pt, e2e1 );
      if ( proj_pt_e1 > 0 && proj_pt_e1 < e2e1Sq )
      {
         return true;
      } 
      
      return false;
   }
   
   return false;
}

void ray_cyl_test(ProtoGraphics& proto, const glm::vec2& norm_mouse){
		ray_t ray;
		ray.origin = vec3(0.f, 0.f, 0.f);
		ray.dir = vec3( cos(norm_mouse.x*3.14f*2.f) * 10.f, 0.f , sin(norm_mouse.x*3.14f*2.f)*10.f);

		proto.setColor( 1.f, 1.f, 1.f );

		proto.drawCone( ray.origin, ray.origin + ray.dir, 0.125f );

		cyl_t cyl;
		cyl.e1 = vec3( 0.f, 0.f, 3.f );
		cyl.e2 = vec3( 5.f, 0.f, 3.f );
		cyl.radi = 0.5f;

		proto.drawCone( cyl.e1, cyl.e2, cyl.radi );

		float t = 0.f;
		bool hit = isect_ray_cyl( ray, cyl, t );

		if ( hit )
			proto.drawSphere( ray.origin + t * ray.dir, 0.25f );
}

bool IsZero( float n ){
	const float kEpsilon = 1e-6f; //
	if ( fabs( n ) <= kEpsilon ) return true;
	return false;
}

//
// taken from the book Essential Maths for Games & Interactive apps by J.M. Van Verth & L.M. Bishop
//
void closestPoints( glm::vec3 &point0, glm::vec3 &point1, float& s0_t, float& s1_t,
					const ray_t &segment0, const ray_t &segment1 ){
    // compute intermediate parameters
    glm::vec3 w0 = segment0.origin - segment1.origin;
    float a = glm::dot( segment0.dir, segment0.dir );
    float b = glm::dot( segment0.dir, segment1.dir );
    float c = glm::dot( segment1.dir, segment1.dir );
    float d = glm::dot( segment0.dir, w0 );
    float e = glm::dot(segment1.dir, w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float s_c, t_c;
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on segment1 to origin0
    if ( ::IsZero(denom) )
    {
        // clamp s_c to 0
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // clamp s_c within [0,1]
        sd = td = denom;
        sn = b*e - c*d;
        tn = a*e - b*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            sn = 0.0f;
            tn = e;
            td = c;
        }
        // clamp s_c to 1
        else if (sn > sd)
        {
            sn = sd;
            tn = e + b;
            td = c;
        }
    }

    // clamp t_c within [0,1]
    // clamp t_c to 0
    if (tn < 0.0f)
    {
        t_c = 0.0f;
        // clamp s_c to 0
        if ( -d < 0.0f )
        {
            s_c = 0.0f;
        }
        // clamp s_c to 1
        else if ( -d > a )
        {
            s_c = 1.0f;
        }
        else
        {
            s_c = -d/a;
        }
    }
    // clamp t_c to 1
    else if (tn > td)
    {
        t_c = 1.0f;
        // clamp s_c to 0
        if ( (-d+b) < 0.0f )
        {
            s_c = 0.0f;
        }
        // clamp s_c to 1
        else if ( (-d+b) > a )
        {
            s_c = 1.0f;
        }
        else
        {
            s_c = (-d+b)/a;
        }
    }
    else
    {
        t_c = tn/td;
        s_c = sn/sd;
    }

    // compute closest points
    point0 = segment0.origin + s_c*segment0.dir;
    point1 = segment1.origin + t_c*segment1.dir;

	s0_t = s_c;
	s1_t = t_c;

}

void closestPointsTest( ProtoGraphics& proto, const glm::vec2& normalized_mouse ){

	static float a = 0.f;
	static float a2 = 0.f;
	if ( proto.mouseDownRight() ) a = normalized_mouse.x*6.28f;
	if ( proto.mouseDownRight() ) a2 = normalized_mouse.y*6.28f;
	

	ray_t line1;
	ray_t line2;

	line1.origin = glm::vec3( 5.f, 0.f, 0.f );
	line1.dir = 20.f * glm::vec3( cos(a2), 0.f, sin(a2) );

	line2.origin = glm::vec3( -5.f, 0.f, 0.f );
	line2.dir = glm::vec3( 0.f, 0.f, -10.f + 20.f * normalized_mouse.x );

	glm::vec3 p1, p2;
	float t1,t2;
	closestPoints( p1, p2, t1, t2, line1, line2 );

	proto.drawSphere( p1, 1.0f );
	proto.drawSphere( p2, 1.0f );

	proto.drawCone( line1.origin, line1.origin + line1.dir, 0.5f);
	proto.drawCone( line2.origin, line2.origin + line2.dir, 0.5f);
}

// proj a onto b
// gives <a,b> / <b,b> , the scalar percentage of a along b. do <a,b>/<b,b> * b to get a vec
float project( const glm::vec3 &a, const glm::vec3 &b ){
	float num = glm::dot(a,b);
	float den = glm::dot(b,b);
	return num / den;
}

namespace StickPhysics{

struct point_t
{
	vec3 pos;
	vec3 old_pos;
	vec3 vel;
	vec3 acc;
	real rmass;
	int collision_group;

	point_t( vec3 pos_, real rmass_ ) : pos(pos_), 
		old_pos(pos_), 
		vel( zero_vec ), 
		acc( zero_vec ), 
		rmass(rmass_),
		collision_group( 0 )
	{

	}
};

// TODO
std::vector<point_t> points;

struct edge_t{
	int p1;
	int p2;
	real length;

	edge_t(int p1_, int p2_) : p1(p1_), p2(p2_) {
		length = glm::distance( points[p1].pos, points[p2].pos );
	}

	bool handle_collision( glm::vec3& impulse0, glm::vec3& impulse1, glm::vec3& impulse2, point_t &other, const glm::vec3 &collision_dir, float distSq ){
			
			point_t &v1 = points[p1];
			point_t &v2 = points[p2];
			// project v0v1 onto v1v2, this projection is used to distribute force onto the line endpoints
			vec3 p_p1 = v1.pos - other.pos;
			const vec3 p1p2 = v2.pos - v1.pos;
			float t = project( p_p1, p1p2 );

			float tot = other.rmass + v1.rmass * (1.f-t) + v2.rmass * t;
			if ( tot <= 0.f ) return false;

			vec3 collision_normal = glm::normalize(collision_dir);
					
			float dist = sqrt( distSq );
			float depth = CYL_RAD+RAD - dist;

			impulse0 = (depth * other.rmass / tot) * collision_normal;
			impulse1 = (depth * v1.rmass*(1.f-t)/tot) * collision_normal;
			impulse2 = (depth * v2.rmass*(    t)/tot) * collision_normal;

			return true;
	}

	bool handle_collision2( glm::vec3& v0_impulse, glm::vec3& v1_impulse, glm::vec3& other_pos, glm::vec3& collision_dir, float distSq, float t )
	{
			point_t &v1 = points[p1];
			point_t &v2 = points[p2];

			// Expects projection of closest points to be defined as point projected onto v1v2
			float tot = v1.rmass * (1.f-t) + v2.rmass * t;
			if ( tot <= 0.f ) return false;

			vec3 collision_normal = glm::normalize(collision_dir);
					
			float dist = sqrt( distSq );
			float depth = 2.f * CYL_RAD - dist;

			v0_impulse = (depth * v1.rmass*(1.f-t)/tot) * collision_normal;
			v1_impulse = (depth * v2.rmass*(    t)/tot) * collision_normal;

			return true;
	}

};

class Engine{
public:

	explicit Engine(){
		init();

	}

	void make_quad( int ia, int ib, int ic, int id ){

		edges.push_back( edge_t ( ia, ib ) );

		edges.push_back( edge_t ( ib, ic ) );

		edges.push_back( edge_t ( ic, id ) );

		edges.push_back( edge_t ( id, ia ) );
	}

	void make_cube(const glm::vec3 &pos, float size ){
		static int coll_group_id = 1;
		float standard_mass = 1.0f;

		int ofs = (int)points.size();

		for(int i=0; i<2; i++){
			for(int j=0; j<2; j++){
				for(int k=0; k<2; k++){
					point_t p ( pos + vec3(i, j, k) * size, standard_mass );
					p.collision_group = coll_group_id;
					points.push_back( p );
				}
			}
		}

		edges.push_back( edge_t(ofs+0,ofs+1) );
		edges.push_back( edge_t(ofs+0,ofs+4) );
		edges.push_back( edge_t(ofs+1,ofs+5) );
		edges.push_back( edge_t(ofs+4,ofs+5) );

		edges.push_back( edge_t(ofs+2,ofs+3) );
		edges.push_back( edge_t(ofs+2,ofs+6) );
		edges.push_back( edge_t(ofs+6,ofs+7) );
		edges.push_back( edge_t(ofs+3,ofs+7) );

		edges.push_back( edge_t(ofs+0,ofs+2) );
		edges.push_back( edge_t(ofs+1,ofs+3) );
		edges.push_back( edge_t(ofs+4,ofs+6) );
		edges.push_back( edge_t(ofs+5,ofs+7) );

		//
		edges.push_back( edge_t(ofs+0,ofs+7) );
		edges.push_back( edge_t(ofs+1,ofs+6) );
		edges.push_back( edge_t(ofs+2,ofs+5) );
		edges.push_back( edge_t(ofs+4,ofs+3) );

		coll_group_id++;

	}

	void make_spiral(){
		float standard_mass = 1.0f;

		float radi = 4.0f;
		float rounds = 4.0f;
		float theta = 0.0f;
		int num_segs = 12*2;
		float ang_inc = (rounds * TWO_PI) / num_segs;
		float y = 0.f;
		for( int segs=0; segs < num_segs; segs++ ) {
			
			float x = cos( theta ) * radi;
			float z = sin( theta ) * radi;
			point_t p1 ( vec3(x, 20.0 + y, z), standard_mass );
			points.push_back( p1 );

			theta += ang_inc;

			//radi += 0.25f;

			y -= 0.125f;	
		}

		points[0].rmass = 0.f; // freeze!

		for ( int i = 0; i< (int)points.size()-1; i++) {
			//edges.push_back( edge_t(points[i+0], points[i+1]) );
			edge_t edge ( i+0, i+1 );
			edges.push_back( edge );
		}
	}

	void make_string( const vec3 &a, const vec3 &b, bool freeze_ends ){
		float dist_spacing = RAD*4.0f;
		float distance = glm::distance( a, b );
		float num_links = floor( (distance / dist_spacing) + 0.5f ) ;

		// make a multiple of 2 points
		if ( fmod(num_links, 2.0f) != 0.0f ){
			num_links++;
		}

		float standard_mass = 1.0f;

		points.push_back( point_t( a, standard_mass ) );

		for( int i=1; i<int(num_links); i++ ){
			float t = i / (num_links-1.0f);
			points.push_back( point_t( glm::mix(a,b,t), standard_mass ) );

			int num_pts = points.size();
			//edge_t edge(points[ num_pts - 2], points[ num_pts - 1]);
			edge_t edge( num_pts - 2, num_pts - 1 );
			edges.push_back( edge );
		}

		if ( freeze_ends ){
			int num_pts = points.size();
			points[ num_pts - (int)num_links].rmass = 0.f;
			points[ num_pts-1 ].rmass = 0.f;
		}

	}

	void init(){
		points.clear();
		edges.clear();

		world_size = 50.0f;

		real standard_mass = (real) 1.0;

		//make_spiral();
		make_string( vec3(-12.f, 0.f, 5.f), vec3(12.f, 0.f, 5.f), true );
		make_string( vec3(-12.f, 0.f, -5.f), vec3(12.f, 0.f, -5.f), true );

		//make_string( vec3(-6.f, 0.f, 0.f), vec3(6.f, 0.f, 0.f), true );
		make_string( vec3(0.f, 2.f, 25.f), vec3(0.f, 2.f, -25.f), false );

		//make_string( vec3(-6.f, 0.f, 0.f), vec3(6.f, 0.f, 0.f), true );

		//make_string( vec3(0.f, 2.f, 4.f), vec3(0.f, 2.f, -4.f), false );

		//make_cube( glm::vec3( 20.f, 20.f, 0.f ), 4.0f );
		//make_cube( glm::vec3( 10.f, 20.f, 0.f ), 4.0f );

		points.push_back( point_t( glm::vec3(20.f, 0.f, 0.f), 1.0f ) );
		points.push_back( point_t( glm::vec3(20.f, 20.f, 0.f), 1.0f ) );
	}

	void update_points(){

		for_each(p_ptr, points){
			point_t &p = *(p_ptr);

			p.acc += (p.rmass * (real) 0.1 / PHYSHZ/2.f) * gravity_dir; // 1e-2
			p.pos += p.acc;
			
			//const real delta = (real)0.995;
			p.vel = (p.pos - p.old_pos); // * delta;

			p.old_pos = p.pos;

			p.pos += p.vel;

			p.acc = zero_vec;

			float half_world_size = world_size / 2.0f;

			for(int i=0; i<3; i++){
				if ( p.pos[i] > half_world_size-RAD ){
					p.pos[i] = half_world_size-RAD;
				}
				if ( p.pos[i] < -half_world_size+RAD ){
					p.pos[i] = -half_world_size+RAD;
				}
			}

			for(int i=0; i<3; i++){

				if ( p.pos[i] <= -half_world_size+RAD || p.pos[i] >= half_world_size-RAD )
					p.pos[i] -= p.vel[i] * 0.5f;
			}

		}
	}

	void update_edges(ProtoGraphics &proto){
		for_each( e_ptr, edges ){
			edge_t &e = *(e_ptr);

			point_t &v1 = points[ e.p1 ];
			point_t &v2 = points[ e.p2 ];
			vec3 p1p2 = v2.pos - v1.pos;
			real dist = glm::length( p1p2 );
			real diff = e.length - dist;
			if ( fabs(diff) > (real)0.0 ){
				vec3 delta = p1p2/dist;
				real pseudo_restiution = diff * 0.5f;
				if (v1.rmass > (real)0.0 ) v1.pos -= delta * pseudo_restiution;
				if (v2.rmass > (real)0.0 ) v2.pos += delta * pseudo_restiution;
			}
		}

	}

	void vertex_vertex_collide(){
		size_t num_points = points.size();
		for(size_t i=0; i<num_points; i++){
			
			for(size_t j=i+1; j<num_points; j++){
				vec3 pipj = points[j].pos - points[i].pos;
				float dsq = glm::dot(pipj, pipj);
				if ( dsq >= pow(RAD*2.f, 2.0f) ) continue;

				float d = sqrt(dsq);
				float f = (RAD*2.f - d) / d;
				float tot = points[i].rmass + points[j].rmass;
				if ( tot <= 0.0f ) continue;

				// impulse   
				//            -(1+e)*vel * N)  
				//         j = ------------
				//             ( rma + rmb )
				// where e is coeff of restitution
				vec3 collision_normal = glm::normalize( pipj );
				points[i].acc -= (f*points[i].rmass/tot) * collision_normal;
				points[j].acc += (f*points[j].rmass/tot) * collision_normal;
			}
		}

	}

	void vertex_edge_collide(ProtoGraphics &proto, bool active){
		for_each( v_ptr , points ){
			point_t &v = *(v_ptr);

			for_each( edge_ptr, edges ){
				edge_t &edge = *(edge_ptr);

				point_t &v1 = points[edge.p1];
				point_t &v2 = points[edge.p2];

				if ( v1.collision_group > 0 ) continue;
				if ( v2.collision_group > 0 ) continue;
				if ( v.collision_group > 0 ) continue;

				// do not collide a vertex against an edge itself is apart of
				if ( &v1 == &v || &v2 == &v ) continue;

				// find the closest point on the line p1-p2 relative to vertex
				const vec3 closest = ProtoMath::closestPtOnLine( v.pos, v1.pos, v2.pos );

				const vec3 collision_dir = v.pos - closest;
				
				
				float distSq = glm::dot( collision_dir, collision_dir );
				float radius_sum = CYL_RAD+RAD;

				if ( distSq < radius_sum*radius_sum ){

					glm::vec3 v0_impulse, v1_impulse, v2_impulse;
					if ( edge.handle_collision( v0_impulse, v1_impulse, v2_impulse, v, collision_dir, distSq ) == false ){
						continue;
					}

					if ( active ){

						v.acc += v0_impulse;
						v1.acc -= v1_impulse;
						v2.acc -= v2_impulse;

					}else{
						proto.setColor(1.f, 1.f, 0.f);
						proto.drawCone( closest, v.pos, 1.0f / 8.f );
						//proto.drawSphere(closest, 0.125f );

						//proto.setColor(0.f, 1.f, 1.f );
						//proto.drawCone( v.pos, v.pos + v_impulse * 100.f, 1.0f / 24.f );

						//proto.setColor(0.f, 0.f, 1.f );
						//proto.drawCone( v1.pos, v1.pos + v1_impulse * 100.f, 1.0f / 24.f );

						//proto.setColor(0.f, 1.f, 0.f );
						//proto.drawCone( v2.pos, v2.pos + v2_impulse * 100.f, 1.0f / 24.f );
					}
				}
			}
		}
	}

	void edge_edge_collide(ProtoGraphics &proto, bool active){
		for( auto e1_ptr=begin(edges); e1_ptr != end(edges); ++e1_ptr ){
			edge_t &ed1 = *(e1_ptr);

			vec3& e1v1 = points[ed1.p1].pos;
			vec3& e1v2 = points[ed1.p2].pos;
			vec3 ed1_v1v2 = e1v2 - e1v1;
			vec3 ed1_dir = glm::normalize(ed1_v1v2); 

			vec3 trunc_e1_v1 = e1v1 + ed1_dir*RAD;
			vec3 trunc_e1_v2 = e1v2 - ed1_dir*RAD;

			ray_t truncated_line1;
			truncated_line1.origin = trunc_e1_v1;
			truncated_line1.dir = trunc_e1_v2 - trunc_e1_v1;

			for( auto e2_ptr=e1_ptr; e2_ptr != end(edges); ++e2_ptr ){
				//e2_ptr++;
				// don't collide edge against self
				if ( e1_ptr == e2_ptr) {
					continue;
				}

				edge_t &ed2 = *(e2_ptr);

				vec3& e2v1 = points[ed2.p1].pos;
				vec3& e2v2 = points[ed2.p2].pos;

				const vec3 ed2_v1v2 = e2v2 - e2v1;
				const vec3 ed2_dir = glm::normalize(ed2_v1v2); 
				const vec3 trunc_e2_v1 = e2v1 + ed2_dir*RAD;
				const vec3 trunc_e2_v2 = e2v2 - ed2_dir*RAD;
				
				ray_t truncated_line2;
				truncated_line2.origin = trunc_e2_v1;
				truncated_line2.dir = trunc_e2_v2 - trunc_e2_v1;

				glm::vec3 hit_e1, hit_e2;
				float e1_t, e2_t;
				closestPoints( hit_e1, hit_e2, e1_t, e2_t, truncated_line1, truncated_line2 );

				glm::vec3 col_dir = hit_e1 - hit_e2;
				float distSq = glm::dot( col_dir, col_dir );
				//float dist = glm::length( col_dir );

				float tot = points[ed1.p1].rmass + points[ed1.p2].rmass + points[ed2.p1].rmass + points[ed2.p2].rmass;
				if ( tot <= 0.0f ) {
					continue;
				}

				float radius = CYL_RAD*2.f;

				if ( distSq < radius*radius ) {

					//glm::vec3 e1_v0_impulse, e1_v1_impulse;
					//if ( ed1.handle_collision2( e1_v0_impulse, e1_v1_impulse, hit_e2, col_dir, distSq, e1_t ) == true ){
					//	points[ed1.p1].acc += e1_v0_impulse;
					//	points[ed1.p2].acc += e1_v1_impulse;
					//}

					//glm::vec3 e2_v0_impulse, e2_v1_impulse;
					//if ( ed2.handle_collision2( e2_v0_impulse, e2_v1_impulse, hit_e1, col_dir, distSq, e2_t ) == true ){
					//	points[ed2.p1].acc -= e2_v0_impulse;
					//	points[ed2.p2].acc -= e2_v1_impulse;
					//}
					//
					if ( !active ){
						float col = proto.sfrand() * 0.5f + 0.5f;
					
						proto.setBlend( true );
						proto.setAlpha( 0.75f );
						proto.setColor( col, 0.f, 0.f );
						//proto.drawSphere( coll_point, RAD*0.125f );
						proto.setColor( 0.f, col, 0.f );
	
						proto.setColor(1.f, 1.f, 0.f);
						proto.drawCone( hit_e1, hit_e2, 0.125f );

					}else{

						//glm::vec3 v0_impulse, v1_impulse, v2_impulse, v3_impulse;
						//if ( ed1.handle_collision2( v0_impulse, v1_impulse, hit_e2, col_dir, distSq, e1_t ) == true ){
						//	points[ed1.p1].acc += v0_impulse;
						//	points[ed1.p2].acc += v1_impulse;
						//}

						//if ( ed2.handle_collision2( v2_impulse, v3_impulse, hit_e1, col_dir, distSq, e2_t ) == true ){
						//	points[ed2.p1].acc -= v2_impulse;
						//	points[ed2.p2].acc -= v3_impulse;
						//}



						float f = (CYL_RAD*2.0f - sqrt(distSq) );
						glm::vec3 coll_normal = glm::normalize( col_dir );
						points[ed1.p1].acc += f * points[ed1.p1].rmass/tot * coll_normal;
						points[ed1.p2].acc += f * points[ed1.p2].rmass/tot * coll_normal;

						points[ed2.p1].acc -= f * points[ed2.p1].rmass/tot * coll_normal;
						points[ed2.p2].acc -= f * points[ed2.p2].rmass/tot * coll_normal;
					}
				}

				if ( !active ) {
					proto.setColor( 1.f, 1.f, 1.f );
					proto.setBlend( false );
				}

			}
		}
	}

	void update(ProtoGraphics &proto){
		vertex_edge_collide(proto, true);
		edge_edge_collide( proto, true );
		vertex_vertex_collide();

		update_edges(proto);	
		update_points();
	}

	void draw(ProtoGraphics &proto){

		//vertex_edge_collide(proto, false);
		edge_edge_collide( proto, false );

		proto.setBlend( true );
		proto.setAlpha( 0.5f );

		proto.setColor(1,0,0);
		for_each( p_ptr, points ){
			proto.drawSphere( p_ptr->pos, RAD );
		}

		proto.setColor(1,1,1);
		for_each( e_ptr, edges ){
			edge_t e = *(e_ptr);

			point_t &v1 = points[ e.p1 ];
			point_t &v2 = points[ e.p2 ];

			vec3 dir = glm::normalize( v2.pos - v1.pos );

			//proto.drawCone( v1.pos, v2.pos, -CYL_RAD );
			proto.drawCone( v1.pos + dir*RAD, v2.pos - dir*RAD, -CYL_RAD );

		}
		proto.setBlend( false );
	}

public:
	float world_size;

private:
	
	std::vector<edge_t> edges;
};

} // end namespace StickPhysics

class Timer{
public:
	typedef std::tr1::function <void(void)> voidfunc;
	explicit Timer(voidfunc callback_, double interval_) : callback(callback_), interval(interval_) {
		start_time = ProtoGraphics::getInstance()->klock();

	}

	void onTick( ) {
		callback();
	}

	void update(){
		double time = ProtoGraphics::getInstance()->klock();
		if ( (time - start_time) > interval ) {
			start_time = time;
			onTick();
		}
	}

private:
	double interval;
	double start_time;
	voidfunc callback;
};


struct IncFunctor
{
	IncFunctor( double &var_ ) : var(var_) {
	}

	void operator() (){
		var++;
	}


	double &var;
};

void draw_line_cube( ProtoGraphics& proto, glm::vec3 pos , float scale )
{
		for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			float u = -1.0f + i*2.0f;
			float v = -1.0f + j*2.0f;
			glm::vec3 p1 ( u, v, -1.f ); glm::vec3 p2 ( u, v, 1.0f );
			p1 *= scale * 0.5f;
			p2 *= scale * 0.5f;

			p1 += pos;
			p2 += pos;

			proto.drawCone( p1, p2 , 1.0 );

			proto.drawCone( p1.xzy(), p2.xzy() , 1.0 );

			proto.drawCone( p1.zxy(), p2.zxy() , 1.0 );
				
		}
	}
}

int main(int argc, const char* argv[])
{
	using glm::vec3;

	ProtoGraphics proto;
	bool inited = proto.init(640,480,argv);
	assert( inited );

	proto.setCamera( vec3(-30.f, -20.f, -60.f), 0,0 );

	//double seconds = 0.0;
	//IncFunctor my_functor( seconds );

	//Timer secTimer( my_functor , 1.0 / 4.0  );


	StickPhysics::Engine my_physics_engine;

	while( proto.isWindowOpen() )
	{
		//secTimer.update();

		proto.cls(0,0,0);

		
		my_physics_engine.draw( proto );

		proto.setColor( 0.0f, 0.0f, 1.0f );
		draw_line_cube( proto, glm::vec3(0.0f), my_physics_engine.world_size );

		static int iter = 0;
		bool run_physics = false;
		if ( proto.keyhit(' ') ){
			std::cout << "iter " << iter++ << std::endl;
			run_physics = true;
		}

		if ( proto.keystatus('E') ){
			std::cout << "iter " << iter++ << std::endl;
			run_physics = true;
		}

		static double tics = 0;
		if ( run_physics ) {
			
			double dt = 1.0/PHYSHZ;
			double tim = proto.klock();

			for(;tics<tim;tics+=dt)
			{
				my_physics_engine.update(proto);
			}
		}else{
			tics = proto.klock();
		}

		if ( proto.keyhit('C') ) my_physics_engine.init();
		
		glm::vec2 norm_mouse( proto.getMouseX() / (float)proto.getWindowWidth(), 1.0f - proto.getMouseY() / (float)proto.getWindowHeight() );
		


		if ( proto.keystatus('G') ){
			float ang2 = 2.0f*3.141592f * norm_mouse.x;
			float ang1 = 2.0f*3.141592f * norm_mouse.y;
			glm::mat4 mtx = glm::rotate( glm::mat4(1.0f), glm::degrees(ang1), vec3(0.0f, 0.0f, 1.0f ) );
			mtx = glm::rotate( mtx, glm::degrees(ang2), vec3(0.0f, 1.0f, 0.0f ) );
			gravity_dir = ( glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f ) * mtx ).xyz();
		}

		//proto.drawCone( zero_vec, gravity_dir*2.0f, 0.1f );

		//closestPointsTest( proto, norm_mouse );
		//closest_pt_test( proto, norm_mouse );

		static int numframes = 0;
		//while( proto.klock() < double(numframes)/(2*60.0) ){ }
		numframes++;

		proto.frame();
	}
	return 0;
}