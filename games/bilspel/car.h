#pragma once;

#include <proto/protographics.h>

glm::vec2 rotate2d(float angle, glm::vec2 v)
{
   float x1 = v.x;
   float y1 = v.y;
   
   float sa = sin(angle);
   float ca = cos(angle);
   
   return glm::vec2( x1 * ca - y1 * sa, 
	                 y1 * ca + x1 * sa );
}



float wedgeProduct( glm::vec2 a, glm::vec2 b )
{
   return a.x * b.y - a.y * b.x;
}


glm::vec2 project( glm::vec2 a, glm::vec2 b )
{
   //projected vector = (a dot b) * b;
   //dpr = a.x * b.x + a.y * b.y;
   float dpr = glm::dot( a,b );
   return b * dpr;
}
glm::vec2 project( glm::vec2 a, glm::vec2 b, float& mag )
{
   //projected vector = (a dot b) * b;
   //dpr = a.x * b.x + a.y * b.y;
   float dpr = glm::dot( a,b );
   mag = dpr;
   return b * dpr;
}

class Car
{
private:
	struct wheel_t
	{
		
		float wheelTorque, wheelSpeed, wheelAngle, wheelInertia, wheelRadius;
		glm::vec2 pos;
		glm::vec2 forwardAxis, sideAxis;

		void init( float x, float y, float radius )
		{
			pos.x = x; pos.y = y;
			wheelTorque = 0.f;
			wheelSpeed = 0.f;
			wheelAngle = 0.f;
			setSteeringAngle( 0 );
			wheelRadius = radius;
			wheelInertia = radius*radius * 2.f; // fake value
		}

		void setSteeringAngle( float angle )
		{
		   forwardAxis = rotate2d( angle, glm::vec2(0.f, 1.f) );
		   sideAxis = rotate2d( angle, glm::vec2(-1.f, 0.f) );
		}
	};
public:
	// Dimensions
	glm::vec2 half_size;

	// Linear
	glm::vec2 pos;
	glm::vec2 vel;
	glm::vec2 forces;
	float mass;
   
	// Angular

	float angle;

	float ang_vel;
	float torque;
	float inertia;
   
	wheel_t wheels[4];
private:
	void init(glm::vec2 half_size, float mass )
	{
		const glm::vec2 zero(0.f);
		pos = zero;
		vel = zero;
		forces = zero;

		angle = 0.f;
		ang_vel = 0.f;
		torque = 0.f;
		this->half_size = half_size;
		this->mass = mass;
		inertia = (1.0 / 12.0) * (half_size.x*half_size.x * half_size.y*half_size.y) * mass;
		// Front wheels
		float wheelRadius = 0.8f;
		wheels[0].init( +half_size.x, +half_size.y, wheelRadius );
		wheels[1].init( -half_size.x, +half_size.y, wheelRadius );
		wheels[2].init( +half_size.x, -half_size.y, wheelRadius );
		wheels[3].init( -half_size.x, -half_size.y, wheelRadius );
	}

	void addForce( glm::vec2 worldForce, glm::vec2 worldOffset )
	{
		forces += worldForce;
		torque += wedgeProduct( worldOffset, worldForce );
	}

	glm::vec2 calculateForce(wheel_t &w, glm::vec2 relativeGroundSpeed, float timestep )
	{
	   // calculate speed of tire patch at ground
	   glm::vec2 patchSpeed = w.forwardAxis * -(w.wheelSpeed*w.wheelRadius);

	   // get vel difference between ground and patch
	   glm::vec2 velDiff = relativeGroundSpeed + patchSpeed;
   
	   // project ground speed onto side axis
	   float forwardMag = 0.0;
	   glm::vec2 sideVel = project( velDiff, w.sideAxis );
   
	   glm::vec2 forwardVel = project(velDiff, w.forwardAxis, forwardMag );
   
	   // Calc super fake dir forces
	   // calc response force
	   glm::vec2 responseForce = sideVel * -2.5f; // TODO make friction decrease with velocity
	   responseForce -= forwardVel; 
   
	   // calc torque on wheel
	   w.wheelTorque += forwardMag * w.wheelRadius;
   
	   // integrate total torque
	   w.wheelSpeed += w.wheelTorque / w.wheelInertia * timestep;

	   w.wheelAngle += w.wheelSpeed * timestep;

	   //clear our transmission torque accumulator
	   w.wheelTorque = 0;

	   return responseForce;
	}

	void updateRigid(float time_step)
	{
	   // Integrate physics linear
	   glm::vec2 accel = forces * 1.0f / mass;
	   vel += accel * time_step;
	   pos += vel * time_step;

	   forces = glm::vec2(0.f); // Clear forces
   
	   //float friction_factor = exp( std::min<float>(1.f, glm::length(vel) ) * time_step * -0.5f ); 
	   //vel *= friction_factor;
   
   
	   // Angular
	   float angAcc = torque / inertia;
	   ang_vel += angAcc * time_step;
	   angle += ang_vel * time_step;
	   torque = 0; // clear torque
	}
public:

	Car()
	{
		float car_width = 2.4f;
		float car_length = 5.f;
		
		//float car_width = 2.6f;
		//float car_length = 4.3f;
		glm::vec2 half_size( 0.5f*car_width, 0.5f*car_length );
		init( half_size, 5.f );
	}

	/// <summary>
	/// set steering position in radians
	/// </summary>
	void setSteering(float steering)
	{
	   float steeringLock = 0.75;
	   wheels[0].setSteeringAngle(-steering * steeringLock);
	   wheels[1].setSteeringAngle(-steering * steeringLock);
	}

	void setThrottle(float throttle)
	{
	   float torque = 20.f; //5
	   // aka AddTransmission torque
	   
	   // Front
	   wheels[0].wheelTorque += throttle * torque; 
	   wheels[1].wheelTorque += throttle * torque; 

	   // Rear
	   //wheels[2].wheelTorque += throttle * torque; 
	   //wheels[3].wheelTorque += throttle * torque;
	}

	void setBrakes(float brakes )
	{
	   float brakeTorque = 1000.0;
	   for(int i=0; i<4; i++)
	   {
		  float wheelVel = wheels[i].wheelSpeed; // aka getWheelSpeed()
		  wheels[i].wheelTorque += -wheelVel * brakeTorque * brakes; //aka AddTransmissionTorque(val);
	   }
	}

	void update(float timestep)
	{
	   for(int i=0; i<4; i++)
	   {
		  glm::vec2 worldWheelOffset = relativeToWorld(wheels[i].pos);
		  glm::vec2 worldGroundVel = pointVel( worldWheelOffset );
		  glm::vec2 relativeGroundSpeed = worldToRelative(-angle, worldGroundVel);
		  glm::vec2 relativeResponseForce = calculateForce( wheels[i], relativeGroundSpeed, timestep );
		  glm::vec2 worldResponseForce = relativeToWorld( relativeResponseForce);
		  addForce( worldResponseForce, worldWheelOffset );
	   }
	   updateRigid( timestep );
	}

	// Velocity of a point on body
	glm::vec2 pointVel( glm::vec2 worldOffset )
	{
		glm::vec2 tangent(  -worldOffset.y * ang_vel, worldOffset.x * ang_vel );
		tangent += vel;
		return tangent;
	}

	glm::vec2 worldToRelative( float angle, glm::vec2 world )
	{
	   return rotate2d( angle, world );
	}

	glm::vec2 relativeToWorld( glm::vec2 relative )
	{
	   return rotate2d( angle, relative );
	}

	glm::vec3 getPosition()
	{
		return glm::vec3( pos.x, wheels[0].wheelRadius, pos.y );
	}

	float getSpeed()
	{
		return glm::length( vel );
	}
};
