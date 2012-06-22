#pragma once;

#define GLM_SWIZZLE 
#include <glm/glm.hpp>
#include "math/math_ray.h"
#include "camera.h"

#include <string>
#include <memory>

namespace protowizard
{
	namespace KEY
	{
		const static int SPECIAL = 256;
		const static int UP = SPECIAL + 27;
		const static int DOWN = SPECIAL + 28;
		const static int LEFT = SPECIAL + 29;
		const static int RIGHT = SPECIAL + 30;
	};


	class Mesh;
	typedef std::shared_ptr<Mesh> MeshPtr;

	class ProtoGraphics;
	typedef std::shared_ptr<ProtoGraphics> ProtoGraphicsPtr;

	class ProtoGraphics
	{
	public:
		ProtoGraphics();
		~ProtoGraphics();

		bool init(int xres, int yres, const std::string &resDir);

		bool isWindowOpen();
		glm::ivec2 getWindowDimensions();
		double klock();
	
		// Debug
		float getMSPF();
		float getAverageMSPF();
		void toggleWireframe() ;
		void debugNormals( bool enable );
		void reloadShaders() ;

		// Input
		glm::vec2 getNormalizedMouse();
		int getMouseX();
		int getMouseY();
		int getMouseWheel();
		void setMouseWheel(int pos);
		bool mouseDownLeft();
		bool mouseDownRight();
		bool keystatus(int key);
		bool keyhit(int key);
		Ray getMousePickRay();

		// Camera
		FirstPersonCamera* getCamera() ;

		// Draw 2D
		void moveTo( float x, float y ) ;
		void lineTo( float to_x, float to_y ) ;
		void drawCircle( float x, float y, float radius ) ;

		// Draw 3D
		void drawSphere( const glm::vec3 &position, float radius ) ;
		void drawCone( const glm::vec3 &p1, const glm::vec3 &p2, float radius ) ;
		void drawPlane( const glm::vec3 &position, const glm::vec3 &normal, float radius ) ;
		void drawCube( const glm::vec3 &position ) ;
		void drawMesh( const glm::vec3 &position, float horiz_ang, float verti_ang, const std::string &path ) ;
		void drawMesh( const glm::vec3 &position, const std::string &path ) ;
		void drawMesh( MeshPtr mesh, bool isTwoSided ) ;

		// Draw state modify
		void setOrientation( const glm::mat4 &ori ) ;
		void setScale( float x, float y, float z ) ;
		void setScale( float uniform_scale ) ;
	
		void setColor( const glm::vec3 &c ) ;
		void setColor( float r, float g, float b ) ;
		void setAlpha( float a ) ;
		void setBlend( bool active ) ;
		void setLightBlend() ;
		void setTexture( const std::string& path ) ;
		void disableTexture() ;
		void clz();
		void cls( float r, float g, float b ) ;

		// Draw Frame!
		void frame() ;

		// Set
		void setFrameRate( int frames_per_second ) ;
		void setTitle( const std::string &str) ;

		// Resource
		std::string getResourceDir() ;
		void setResourceDir( const std::string& new_dir ) ;

	private:
		class impl;
		std::unique_ptr<impl> pimpl;
	};
} // end ::proto
