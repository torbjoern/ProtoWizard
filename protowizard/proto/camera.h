#pragma once

#include <glm/glm.hpp>

namespace protowizard
{
class FirstPersonCamera
{
public:
	void lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up );

	void update(bool left_key, bool right_key, bool back_key, bool forwards_key,
		float mouse_x, float mouse_y, bool mouse_is_down, float delta );

	float getHorizontalAngleDegrees() { return horiz_deg; }

	float getVerticalAngleDegrees() { return verti_deg; }

	glm::vec3 getStrafeDirection() const { return glm::vec3(view[0].x, view[1].x, view[2].x); }

	glm::vec3 getUpDirection() const { return glm::vec3(view[0].y, view[1].y, view[2].y); }

	glm::vec3 getLookDirection() const { 
        // compensate for glm::lookAt reversing zDir as of GL convention
        return -glm::vec3(view[0].z, view[1].z, view[2].z); 
    }

	float getFov(){return vFov;}
	void setFov( float vFov ){ this->vFov = vFov;}

    void setNearDist(float near_dist) { this->near_dist = near_dist; }
	float getNearDist() { return near_dist; }

	float getFarDist() { return far_dist; }
	void setFarDist(float far_dist) { this->far_dist = far_dist; }

    void setPos(const glm::vec3& pos ) { this->pos = pos; };
    const glm::vec3 &getPos() const { return pos; }

	const glm::mat4 &updateProjection(int w, int h);
	const glm::mat4 &getProjection() const { return projection; }
    glm::mat4 getViewMatrix() const { return view; }

	FirstPersonCamera();
private:
	glm::mat4 projection;
    glm::mat4 view;
	
	glm::vec3 pos;

	float vFov;
	
    float near_dist;
	float far_dist;

    float movementUnitsPerSecond;
    float mouseDegreesPerSecond;

    float horiz_deg;
    float verti_deg;
	float oldmousx;
	float oldmousy;
};
}