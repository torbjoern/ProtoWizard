#pragma once;

// Forward decls
#include "camera.h"
//class FirstPersonCamera;

class TextureManager;
class MeshManager;

struct LineSegmentState;
struct CircleState;

struct BaseState3D;
struct SphereState;
struct CylinderState;
struct PlaneState;
struct CubeState;
struct MeshState;

class Shader;

#include <vector>

#define GLM_SWIZZLE 
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp> // Value ptr -> column-ordered pointer to GLM type

#include "math/math_ray.h"
#include "math/math_common.h"

class ProtoGraphics
{
private:
	typedef std::shared_ptr<BaseState3D> BaseState3DPtr;

private:
	// Disallowing copying. Please pass protographics about as a const ptr or refrence!
	ProtoGraphics(const ProtoGraphics&); // no implementation 
	ProtoGraphics& operator=(const ProtoGraphics&); // no implementation 
public:
	ProtoGraphics();
	~ProtoGraphics();

	double octaves_of_noise(int octaves, double x, double y, double z);

	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);

	float getMSPF();
	float getAverageMSPF();
	

	void dump_stats();

	bool init(int xres, int yres, const char* argv[] );

	void setCamera( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up );

	void setCamera( float x, float y, float z, float hang, float vang );

	void setCamera( const glm::vec3& pos, float hang, float vang );

	FirstPersonCamera* getCamera() {return camera; }

	void setTitle( const std::string &str);

	void setTexture( const std::string& path );

	void disableTexture();

	void shutdown();

	void cls( float r, float g, float b );

	bool isWindowOpen();

	int getWindowWidth();

	int getWindowHeight();

	static ProtoGraphics* getInstance() { return instance; }
	
	double klock();

	std::string getResourceDir() { return resource_dir; }
	void setResourceDir( const std::string& new_dir ) { resource_dir = new_dir; }

	int getMouseX();

	int getMouseY();

	int getMouseWheel();

	protomath::Ray getMousePickRay();

	/// <summary>
	/// returns mousx / xres, a value within [0,1]
	/// </summary>
	float getNormalizedMouseX(); 

	/// <summary>
	/// returns mousy / yres, a value within [0,1]
	/// </summary>
	float getNormalizedMouseY(); 

	bool mouseDownLeft();

	bool mouseDownRight();

	bool keystatus(int key);

	bool keyhit(int key);

	void setColor( glm::vec3 c );

	void setColor( float r, float g, float b );

	void setAlpha( float a );

	void setOrientation( const glm::mat4 &ori );

	void setScale( float x, float y, float z );

	void setScale( float uniform_scale );

	void setBlend( bool active );

	void setLightBlend();

	void setFrameRate( int frames_per_second );

	void moveTo( float x, float y );

	void lineTo( float to_x, float to_y );

	void drawCircle( float x, float y, float radius );

	void drawSphere( glm::vec3 position, float radius );

	void drawCone( glm::vec3 p1, glm::vec3 p2, float radius );

	/// <summary>
	/// draw a cube centered at position, with diameter 1.0 given setScale( 1.0 )
	/// </summary>
	void drawCube( glm::vec3 position );

	void drawPlane( glm::vec3 position, glm::vec3 normal, float radius );

	// Draw mesh, create transformation from two angles, horizontal and vertical rotation
	void drawMesh( glm::vec3 pos, float hang, float vang, std::string path );

	// Draw mesh using current transformation matrix
	void drawMesh( glm::vec3 pos, std::string path );

	void frame();

	void debugNormals( bool enable );
private:

	void initState();

	void handle_key(int key, int action);

	static void _key_callback(int key, int action)
	{
		instance->handle_key(key, action);
	}

	static int _closeCallback(void)
	{
		// dont shut down ProtoGraphics yet. Allow its dtor to run
		instance->isRunning = false;
		
		return 0; // Do not close OpenGL context yet...
	}

	void draw_buffered_lines();

	void draw_buffered_circles();

	void draw_buffered_shapes( const Shader& shader );

	void draw_buffered_objects();

	void init_phong( const Shader& active_shader );

	bool install_shaders();

	glm::mat4 get3DTransform(const glm::mat4& orientation, const glm::vec3& position, const glm::vec3 scale );
	void save_state( BaseState3DPtr state, const glm::mat4& transform );

	// TODO.. use friend instead...?
public:
	MeshManager *mesh_manager;
private:
	static ProtoGraphics *instance;
	bool isRunning;
	bool hasShutdown;

	int xres, yres;
	int mousx, mousy;

	static const int NUM_KEYS = 325;
	bool key_array[NUM_KEYS];
	int key_hit_array[NUM_KEYS];

	glm::vec4 colorState;
	glm::vec2 move_to_state;
	int blend_state;
	glm::mat4 currentOrientation;
	glm::vec3 currentPosition; // TODO. basically moveTo in 3D
	glm::vec3 scale;

	glm::vec3 light_pos;

	FirstPersonCamera *camera;
	TextureManager *texture_manager;
	
	
	Shader *shader_2d;
	Shader *shader_lines2d;
	Shader *phong_shader;
	Shader *geo_shader_normals;

	std::vector< Shader* > shader_list;
	
	std::vector< LineSegmentState > buffered_lines;
	std::vector< CircleState > buffered_circles;

	std::vector<BaseState3DPtr> opaque;
	std::vector<BaseState3DPtr> translucent;

	double delta_time;
	double time;
	double old_time;
	double max_millis_per_frame;

	unsigned int numframes;

	int num_opaque;
	int num_blended;

	bool isDebugNormalsActive;

	std::string resource_dir;

	double mspf_samples[10];
	int currentSample;
};

//};