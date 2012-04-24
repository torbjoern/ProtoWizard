#pragma once;

#define GLM_SWIZZLE 
#include <glm/glm.hpp>
#include "math/math_ray.h"
#include "camera.h"

#include <string>
#include <memory>
class ProtoGraphics;
typedef std::shared_ptr<ProtoGraphics> ProtoGraphicsPtr;


class ProtoGraphics
{
private:
	
public:
	//ProtoGraphics() {}
	virtual ~ProtoGraphics() {}
	static ProtoGraphicsPtr create();

	virtual bool init(int xres, int yres, const char* argv[] ) = 0;

	virtual float getMSPF()= 0;
	virtual float getAverageMSPF()= 0;
	virtual bool isWindowOpen()= 0;
	virtual int getWindowWidth()= 0;
	virtual int getWindowHeight()= 0;
	virtual double klock()= 0;
	
	// Debug
	virtual void toggleWireframe() = 0;
	virtual void debugNormals( bool enable )= 0;
	virtual void reloadShaders() = 0;

	// Input
	virtual float getNormalizedMouseX()= 0;
	virtual float getNormalizedMouseY()= 0;
	virtual int getMouseX()= 0;
	virtual int getMouseY()= 0;
	virtual int getMouseWheel()= 0;
	virtual bool mouseDownLeft()= 0;
	virtual bool mouseDownRight()= 0;
	virtual bool keystatus(int key)= 0;
	virtual bool keyhit(int key)= 0;
	virtual protomath::Ray getMousePickRay()= 0;

	// Camera
	virtual void setCamera( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up ) = 0;
	virtual void setCamera( const glm::vec3& pos, float hang, float vang ) = 0;
	virtual FirstPersonCamera* getCamera() = 0;

	// Draw 2D
	virtual void moveTo( float x, float y ) = 0;
	virtual void lineTo( float to_x, float to_y ) = 0;
	virtual void drawCircle( float x, float y, float radius ) = 0;

	// Draw 3D
	virtual void drawSphere( glm::vec3 position, float radius ) = 0;
	virtual void drawCone( glm::vec3 p1, glm::vec3 p2, float radius ) = 0;
	virtual void drawPlane( glm::vec3 position, glm::vec3 normal, float radius ) = 0;
	virtual void drawCube( glm::vec3 position ) = 0;
	virtual void drawMesh( glm::vec3 position, float horiz_ang, float verti_ang, std::string path ) = 0;
	virtual void drawMesh( glm::vec3 position, std::string path ) = 0;

	// Draw state modify
	virtual void setOrientation( const glm::mat4 &ori ) = 0;
	virtual void setScale( float x, float y, float z ) = 0;
	virtual void setScale( float uniform_scale ) = 0;
	
	virtual void setColor( glm::vec3 c ) = 0;
	virtual void setColor( float r, float g, float b ) = 0;
	virtual void setAlpha( float a ) = 0;
	virtual void setBlend( bool active ) = 0;
	virtual void setLightBlend() = 0;
	virtual void setTexture( const std::string& path ) = 0;
	virtual void disableTexture() = 0;
	virtual void clz() = 0;
	virtual void cls( float r, float g, float b ) = 0;

	// Draw Frame!
	virtual void frame() = 0;

	// Set
	virtual void setFrameRate( int frames_per_second ) = 0;
	virtual void setTitle( const std::string &str) = 0;
	// Resource
	virtual std::string getResourceDir() = 0;
	virtual void setResourceDir( const std::string& new_dir ) = 0;

};
