
#include "proto/camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace protowizard;

const glm::mat4 &FirstPersonCamera::updateProjection(int w, int h)
{
	projection = glm::perspective<float>(vFov, w/(float)h, near_dist, far_dist);
	return projection;
}

float angleBetween(const glm::vec3& v1, const glm::vec3& v2)
{
   float ftol = 1e-6f;
   if( fabs(v1[0]-v2[0]) < ftol || fabs(v1[1]-v2[1]) < ftol || fabs(v1[2]-v2[2]) < ftol) return 0;
   
   return acos( glm::dot(v1,v2) / (glm::length(v1)*glm::length(v2)) );
}

void FirstPersonCamera::lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up )
{
	setPos(pos);
	view = glm::lookAt( pos, target, up );

	// not 0,0,1
	// not 0,0,-1
	horiz_deg = glm::angle( getLookDirection(), glm::vec3(0.f, 0.f, -1.f) );
	verti_deg = glm::angle( getStrafeDirection(), glm::vec3(-1.f, 0.f, 0.f) ) ;//- 3.14;
	//horiz_deg = glm::degrees( angleBetween( getLookDirection(), glm::vec3(0.f, 0.f, 1.f) ) );
	//verti_deg = glm::degrees( angleBetween( getStrafeDirection(), glm::vec3(1.f, 0.f, 0.f) ) );
}

void FirstPersonCamera::update(bool left_key, bool right_key, bool back_key, bool forwards_key,
	float mouse_x, float mouse_y, bool mouse_is_down, float delta )
{
	float mouse_speed_x = (delta * mouseDegreesPerSecond) * (mouse_x - oldmousx);
	float mouse_speed_y = (delta * mouseDegreesPerSecond) * (mouse_y - oldmousy);
	oldmousx = mouse_x;
	oldmousy = mouse_y;
	if ( mouse_is_down ) {	
        horiz_deg += mouse_speed_x;
		verti_deg += mouse_speed_y;
	}
	float speed = delta * movementUnitsPerSecond; // meters per second

	float xMove = float(left_key-right_key);
    float zMove = float(forwards_key-back_key);
#if 0
	// www.opengl-tutorial.org/ solution
	// rotation angles become reversed
	// is this solution any more correct than mine?
    float horiz_rad = glm::radians(horiz_deg);
    float verti_rad = glm::radians(verti_deg);

	// Z-dir vector from result of mult X and Y rotation matrices
	glm::vec3 lookDir(
        cos(verti_rad) * sin(horiz_rad),
        sin(verti_rad),
        cos(verti_rad) * cos(horiz_rad)
    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horiz_rad - 3.14f/2.0f),
        0,
        cos(horiz_rad - 3.14f/2.0f)
    );

    glm::vec3 up = glm::cross( right, lookDir );

    glm::vec3 moveDir = right*xMove + lookDir*zMove;
    if ( glm::length2(moveDir) ) 
        pos += speed * glm::normalize(moveDir);

    view = glm::lookAt( pos, pos+lookDir, up );
#else
	view = glm::rotate( glm::mat4(1.0f), verti_deg, glm::vec3(1.f, 0.f, 0.f) );
	view = glm::rotate( view,            horiz_deg, glm::vec3(0.f, 1.f, 0.f) );

	glm::vec4 moveDir( speed * xMove, 0.0f, speed * zMove, 0.0f ); // untransformed translate vec
	glm::vec4 lookDir = moveDir * view;						       // transform it by look rotation
	pos += glm::vec3( lookDir );                                   // next desired pos is current + dir

	view = glm::translate(view, pos);
#endif
}				  					  

FirstPersonCamera::FirstPersonCamera() 
    : projection ( glm::mat4(1.f) )
    ,view( glm::mat4(1.f) )
    ,pos( glm::vec3(0.f) )
    ,vFov (45.f)
    ,near_dist (.5f)
	,far_dist (1000.f)
    ,movementUnitsPerSecond (50.f)
    ,mouseDegreesPerSecond(35.f)
    ,horiz_deg (0.f)
    ,verti_deg (0.f)
    ,oldmousx (0.f)
    ,oldmousy (0.f)
{
}