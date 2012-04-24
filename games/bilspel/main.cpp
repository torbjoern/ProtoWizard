#include <proto/protographics.h>
#include <proto/math/math_common.h>
#include <proto/color_utils.h>
#include <proto/fileio/text_file.h>

#include <glm/gtc/quaternion.hpp>
#include "../../depends/obj_loader/objLoader.h"

#include <iostream>
#include <string>

#include "car.h"

namespace KEY
{
	const static int  SPECIAL = 256;
	const static int  UP = SPECIAL + 27;
	const static int  DOWN = SPECIAL + 28;
	const static int  LEFT = SPECIAL + 29;
	const static int  RIGHT = SPECIAL + 30;
};

namespace // GLOBALS
{
	double time = 0.0;
	float time_step = 1.f / 60.f;
	ProtoGraphicsPtr proto;
}

void draw_level()
{
	proto->setColor(0.9f, 0.9f, 0.9f);
	proto->setTexture( proto->getResourceDir() + "/textures/lyngaas.png");

	proto->setOrientation( glm::rotate(identityMatrix, -90.f, unitX) );
	proto->drawMesh( glm::vec3(0.f, 0.f, 0.f), proto->getResourceDir() + "/models/terrain.obj");
	proto->disableTexture();
}

void control_car( Car &car )
{
	if ( proto->keyhit('R') )
	{
		car.pos = glm::vec2(0.f,0.f);
		car.vel = glm::vec2(0.f,0.f);
		car.angle = 0.f;
		car.ang_vel = 0.f;

		for (int i=0; i<4; i++ ){
			car.wheels[i].wheelTorque = 0.f;
			car.wheels[i].wheelSpeed = 0.f;
			car.wheels[i].wheelAngle = 0.f;
		}
	}
	float steering = float( proto->keystatus('A') - proto->keystatus('D') );
	float throttle = float( proto->keystatus('W') - proto->keystatus('S') );
	

	car.setSteering( steering );
	car.setThrottle( throttle );
	if ( proto->keystatus(' ') ) car.setBrakes( time_step );
	car.update( time_step );
}

void draw_car( Car &car )
{
	glm::mat4 model_offset = glm::rotate(identityMatrix, -90.f, unitX);
	glm::mat4 car_ori = glm::rotate(identityMatrix, glm::degrees(-car.angle), unitY );
	proto->setOrientation( car_ori * model_offset );
	proto->setColor(63/255.f, 72/255.f, 200/255.f);
	proto->drawMesh( car.getPosition(), proto->getResourceDir() + "/models/volvo.obj" );

   for(int i=0; i<4; i++)
   {
      glm::vec2 worldWheelOffset = car.relativeToWorld( car.wheels[i].pos);
      glm::vec2 worldWheelSideVec = car.relativeToWorld( car.wheels[i].sideAxis);
      glm::vec2 worldWheelFwdVec = car.relativeToWorld( car.wheels[i].forwardAxis);
	  worldWheelOffset += car.pos;
	  worldWheelSideVec *= car.wheels[i].wheelSpeed;

      if ( car.wheels[i].wheelSpeed > 0 )
      {
         proto->setColor(1.f,1.f,0.f);
      }else{
         proto->setColor(1.f,0.f,0.f);
      }

      float wheelTurn = atan2(  car.wheels[i].forwardAxis.y -car.wheels[i].sideAxis.y,  car.wheels[i].forwardAxis.x - car.wheels[i].sideAxis.x );
      
	  glm::vec3 wheel_pos( worldWheelOffset.x, car.wheels[i].wheelRadius, worldWheelOffset.y );
	  float ang_y_axis = -M_PI/4+car.angle + wheelTurn; // 
	  float ang_x_axis = car.wheels[i].wheelAngle / M_PI;

	  glm::mat4 wheel_ori = glm::rotate(identityMatrix, glm::degrees(-ang_y_axis), unitY ) * glm::rotate(identityMatrix, glm::degrees(ang_x_axis), unitX );
	  proto->setOrientation( wheel_ori );
	  proto->setScale( car.wheels[i].wheelRadius*2.f );
	  proto->drawMesh( wheel_pos, proto->getResourceDir() + "/models/wheel.obj" );
	  proto->setColor(0.2f, 0.2f, 0.2f);
	  proto->drawMesh( wheel_pos, proto->getResourceDir() + "/models/tyre.obj" );
	  proto->setScale(1.f);
	  
	  bool debug_draw = true;
	  if ( debug_draw )
	  {
		glm::vec2& fwd = car.wheels[i].forwardAxis;
		glm::vec2& side = car.wheels[i].sideAxis;
		//proto->setBlend(true);
		proto->setAlpha(0.75f);

		glm::vec3 fwdDir(fwd.x, 0.f, fwd.y);
		glm::vec3 sideDir(side.x, 0.f, side.y);
		if ( i == 0 || i == 2 ) {
			fwdDir.x *= -1;
			sideDir.x *= -1;
		}
		
		glm::vec3 fwdArrow = 2.f * glm::vec3(car_ori * glm::vec4(fwdDir,1.0f) );
		glm::vec3 sideArrow = 2.f * glm::vec3(car_ori * glm::vec4(sideDir,1.0f) );

		proto->setColor(1.f,0.1f,0.1f); proto->drawCone( wheel_pos, wheel_pos + fwdArrow, 0.1f );
		proto->setColor(0.1f,0.1f,1); proto->drawCone( wheel_pos, wheel_pos + sideArrow, 0.1f );
		proto->setBlend(false);
	  }
   }
}

void debug_matrix(const glm::mat4& m)
{
	for (int i=0; i<4; i++) {
	printf("%.3f %.3f %.3f %.3f \n", m[i].x, m[i].y, m[i].z, m[i].w );
}
}

int main(int argc, const char* argv[])
{
	proto = ProtoGraphics::create();

	if (!proto->init(640,480,argv) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}

	proto->setFrameRate( 60 );
	//proto->debugNormals(true);

	Car my_car;

	proto->setCamera( glm::vec3(0.f, -30.f, 0.f), 0.f, 0.f );
	glm::mat4 cam_frame = proto->getCamera()->getViewMatrix();
	debug_matrix( cam_frame );
	do
	{
		time = proto->klock();

		proto->cls(0.f, 0.f, 0.f);

		float normalized_mx = proto->getMouseX() / (float)proto->getWindowWidth();
		float normalized_my = proto->getMouseY() / (float)proto->getWindowHeight();
		glm::vec2 normalized_mouse(normalized_mx, normalized_my);
				
		
		glm::vec3 carDir = 0.25f * glm::normalize(glm::vec3( my_car.vel.x, 0.f, my_car.vel.y )) + glm::vec3( cos(my_car.angle+M_PI*0.5f), 0.f, sin(my_car.angle+M_PI*0.5f) );
		//carDir = glm::normalize(carDir);
		
		static bool fps_cam_mode = true;
		
		if ( proto->keyhit('C') ) {
			if ( fps_cam_mode ) {
				printf("save fps cam\n");
				cam_frame = proto->getCamera()->getViewMatrix();
				debug_matrix( cam_frame );
			}
			fps_cam_mode =!fps_cam_mode;
			if ( fps_cam_mode ) { 
				printf("restore fps cam\n");
				proto->getCamera()->setViewMatrix(cam_frame);
				debug_matrix( cam_frame );
			}
		}

		if ( fps_cam_mode ) 
		{
			proto->getCamera()->update( proto->keystatus(KEY::LEFT), proto->keystatus(KEY::RIGHT), proto->keystatus(KEY::UP), proto->keystatus(KEY::DOWN),
									   (float)proto->getMouseX(), (float)proto->getMouseY(), proto->mouseDownLeft(), proto->getMSPF() );
		} else {
			
			proto->setCamera( my_car.getPosition() + glm::vec3(0.f, 5.f,0.f) - carDir*5.f, my_car.getPosition()+ glm::vec3(0.f, 2.f,0.f)+5.f*carDir, glm::vec3(0.f, 1.f, 0.f) );
		}

		control_car( my_car );
		draw_car( my_car );
		draw_level();

		proto->frame();
		
	} while( proto->isWindowOpen() );


	return 0;

}