#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/glm.hpp>

class FirstPersonCamera
{
public:
	FirstPersonCamera();

	void lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up );

	void set( glm::vec3 const& new_pos, float horiz_degrees, float verti_degrees ) ;

	void update(bool left_key, bool right_key, bool back_key, bool forwards_key,
		float mouse_x, float mouse_y, bool mouse_is_down, float delta );

	float getHorizontalAngle() {
		return hang;
	}

	float getVerticalAngle() {
		return vang;
	}

	glm::vec3 getStrafeDirection() 
	{
		return glm::vec3( mCam[0].x, mCam[1].x, mCam[2].x );
	}

	glm::vec3 getUpDirection() 
	{
		return glm::vec3( mCam[0].y, mCam[1].y, mCam[2].y );
	}

	glm::vec3 getLookDirection() 
	{
		return -1.f * glm::vec3( mCam[0].z, mCam[1].z, mCam[2].z );
	}

	float getFov(){return fov;}
	glm::mat4 getViewMatrix();
	void setViewMatrix( const glm::mat4& mat ) { mCam = mat; }
	void setPos(const glm::vec3& pos );
	glm::vec3 getPos();

private:
	float hang, vang;
	glm::mat4 mCam;
	glm::vec3 pos;
	float fov;

	float oldmousx;
	float oldmousy;
};

#endif