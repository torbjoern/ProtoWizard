
#include "camera.h"

#include "common.h"
#include <glm/gtc/quaternion.hpp>

FirstPersonCamera::FirstPersonCamera()
{
	mCam = identityMatrix;
	pos = glm::vec3(0.f);
	fov = 90.f;

	hang = 0.0f;
	vang = 0.0f;

	oldmousx = 0.0f;
	oldmousy = 0.0f;
}

void FirstPersonCamera::lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up )
{
	setPos(pos);
	mCam = glm::lookAt( pos, target, up );
}

glm::vec3 SphericalToCartesian(float polar, float elevation){
	float radius = 1.f;
	float a = radius * cos(elevation);
	return glm::vec3( a * cos(polar), radius * sin(elevation), a * sin(polar) );
}

void FirstPersonCamera::set( glm::vec3 const& pos, float horiz_degrees, float verti_degrees ) {
	setPos(pos);
	glm::vec3 target = SphericalToCartesian(hang,vang); 
	mCam = glm::lookAt( pos, target, glm::vec3(0.f, 1.f, 0.f) );
}

//void FirstPersonCamera::quat_code()
//{
//	glm::quat q_rotate;
//	q_rotate = glm::rotate( q_rotate, hang, glm::vec3( 0, 1, 0 ) );
//	q_rotate = glm::rotate( q_rotate, -vang, glm::vec3( 1, 0, 0
//}

void FirstPersonCamera::update(bool left_key, bool right_key, bool back_key, bool forwards_key,
	float mouse_x, float mouse_y, bool mouse_is_down, float delta )
{
	float mouse_speed_x = mouse_x - oldmousx;
	float mouse_speed_y = mouse_y - oldmousy;
	oldmousx = mouse_x;
	oldmousy = mouse_y;
	if ( mouse_is_down )
	{	
		hang += mouse_speed_x * 0.5f;
		vang += mouse_speed_y * 0.5f;
	}
	float speed = delta * 50.0f; // meters per second

	glm::mat4 rmx = glm::rotate( identityMatrix, vang, glm::vec3(1.f, 0.f, 0.f)  );
	glm::mat4 rmy = glm::rotate( identityMatrix, hang, glm::vec3(0.f, 1.f, 0.f)  );
	glm::mat4 rotMat = rmx * rmy;

	glm::vec4 moveDir( speed * (left_key-right_key), 0.0f, speed * (back_key - forwards_key), 0.0f );
	glm::vec4 lookDir = moveDir * rotMat;
	glm::vec3 newPos = getPos() + glm::vec3( lookDir );

	setPos(newPos);
	mCam = glm::translate(rotMat, newPos);
}

glm::mat4 FirstPersonCamera::getViewMatrix()
{
	return mCam;
}

void FirstPersonCamera::setPos(const glm::vec3& pos ) {
	/*mCam[3].x = pos.x;
	mCam[3].y = pos.y;
	mCam[3].z = pos.z;*/
	this->pos = pos;
}

glm::vec3 FirstPersonCamera::getPos(){
	return pos;
	//return glm::vec3( mCam[3].x, mCam[3].y, mCam[3].z );
}					  
					  