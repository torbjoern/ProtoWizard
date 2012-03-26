
#include "camera.h"

#include "common.h"

FirstPersonCamera::FirstPersonCamera()
{
	mCam = glm::mat4(1.0);


	glm::vec3 zero(0.0f);
	pos = zero;
	dirvec = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	fov = 90.f;

	hang = 0.0f;
	vang = 0.0f;

	oldmousx = 0.0f;
	oldmousy = 0.0f;

	calcViewMatrix();
}

void FirstPersonCamera::calcViewMatrix() 
{
	glm::mat4 rmx = glm::rotate( glm::mat4(1.0), vang, glm::vec3(1.f, 0.f, 0.f)  );
	glm::mat4 rmy = glm::rotate( glm::mat4(1.0), hang, glm::vec3(0.f, 1.f, 0.f)  );
	mCam = glm::translate( rmx * rmy, pos );
}

void FirstPersonCamera::lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up )
{
	mCam = glm::lookAt( pos, target, up );
	this->pos = pos;
}

void FirstPersonCamera::set( glm::vec3 const& new_pos, float horiz_degrees, float verti_degrees ) {
	pos = new_pos;
	hang = horiz_degrees;
	vang = verti_degrees;

	calcViewMatrix();
}

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

	dirvec = glm::vec4(  speed * (left_key-right_key), 0.0f, speed * (back_key - forwards_key), 1.0f );
		
	glm::mat4 rmx = glm::rotate( glm::mat4(1.0), vang, glm::vec3(1.f, 0.f, 0.f)  );
	glm::mat4 rmy = glm::rotate( glm::mat4(1.0), hang, glm::vec3(0.f, 1.f, 0.f)  );
	glm::mat4 rotmat = rmx * rmy;

	mCam = rotmat;
	glm::vec4 newdir = dirvec*rotmat;
	pos += glm::vec3( newdir.x, newdir.y, newdir.z );
	mCam = glm::translate( mCam, pos );
}

glm::mat4 FirstPersonCamera::getViewMatrix()
{
	return mCam;
}

glm::vec3 FirstPersonCamera::getPos(){
	return pos;
}
