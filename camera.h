#ifndef _CAMERA_H
#define _CAMERA_H

#include "common.h"

class FirstPersonCamera
{
public:
	glm::mat4 mCam;
	glm::vec3 pos;
	glm::vec4 dirvec;
	float hang, vang;
	float oldmousx;
	float oldmousy;

	FirstPersonCamera()
	{
		mCam = glm::mat4(1.0);


		glm::vec3 zero(0.0f);
		pos = zero;
		dirvec = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		hang = 0.0f;
		vang = 0.0f;

		oldmousx = 0.0f;
		oldmousy = 0.0f;
	}


	void update(bool left_key, bool right_key, bool back_key, bool forwards_key,
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

		dirvec = glm::vec4( delta * (left_key-right_key), 0.0f,  delta * (back_key - forwards_key), 1.0f );
	}

	glm::mat4 getViewMatrix()
	{
		glm::mat4 rmx = glm::rotate( glm::mat4(1.0), vang, glm::vec3(1.f, 0.f, 0.f)  );
		glm::mat4 rmy = glm::rotate( glm::mat4(1.0), hang, glm::vec3(0.f, 1.f, 0.f)  );
		glm::mat4 rotmat = rmx * rmy;

		mCam = rotmat;
		glm::vec4 newdir = dirvec*rotmat;
		glm::vec3 newpos = pos + glm::vec3( newdir.x, newdir.y, newdir.z );
		pos = newpos;
		mCam = glm::translate( mCam, newpos );
		return mCam;
	}




};

#endif
