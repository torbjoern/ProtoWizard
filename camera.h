#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/glm.hpp>

class FirstPersonCamera
{
public:
	FirstPersonCamera();

	/* converts horizontal and vertical angles to a 3x3 rotation matrix */
	static glm::mat3 ang2mat(float hang, float vang )
	{
		//glm::vec3 side( cos(hang), 0.f, -sin(hang) );
		//glm::vec3 fwd( cos(vang)*sin(hang), sin(vang), cos(vang)*cos(hang) );

		glm::vec3 side( cos(hang), 0.f, sin(hang) );
		glm::vec3 fwd( cos(hang)*-sin(vang), sin(hang), cos(hang)*cos(vang) );

		glm::vec3 up = glm::cross( fwd, side );
		return glm::mat3( side, up, fwd );
	}

	void lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up );

	void set( glm::vec3 const& new_pos, float horiz_degrees, float verti_degrees ) ;

	void update(bool left_key, bool right_key, bool back_key, bool forwards_key,
		float mouse_x, float mouse_y, bool mouse_is_down, float delta );

	glm::mat4 getViewMatrix();

	glm::vec3 getPos();

	float getHorizontalAngle(){
		return hang;
	}

	float getVerticalAngle(){
		return vang;
	}

private:
	void calcViewMatrix();

private:
	float hang, vang;
	glm::mat4 mCam;
	glm::vec3 pos;
	glm::vec4 dirvec;

	float oldmousx;
	float oldmousy;


};

#endif
