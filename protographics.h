



#include "camera.h"

#include "shapes/shapes.h"


template <class T> T DEGREES_TO_RADIANS(T degrees)
{
	return degrees * T(M_PI) / T(180.f);
}

template <class T> T RADIANS_TO_DEGREES(T radians)
{
	return radians / T(M_PI) * T(180.f);
}


//namespace Proto{

class ProtoGraphics
{
private:
	// Disallowing copying. Please pass protographics about as a const ptr or refrence!
	ProtoGraphics(const ProtoGraphics&); // no implementation 
	ProtoGraphics& operator=(const ProtoGraphics&); // no implementation 

public:
	ProtoGraphics();
	~ProtoGraphics();

	// fast float random in interval -1,1
	// source by RGBA: http://www.rgba.org/articles/sfrand/sfrand.htm
	float sfrand( void );

	float random( float range_begin, float range_end );

	double octaves_of_noise(int octaves, double x, double y, double z);

	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);

	float getMSPF();

	void dump_stats();

	bool init(int xres, int yres);

	void setCamera( glm::vec3 pos, glm::vec3 target, glm::vec3 up );

	void setCamera( float x, float y, float z, float hang, float vang );

	void setCamera( glm::vec3 pos, float hang, float vang );

	void setTitle( const std::string &str);

	void shutdown();

	void cls( float r, float g, float b );

	bool isWindowOpen();

	int getWindowWidth();

	int getWindowHeight();

	static ProtoGraphics* getInstance() { return instance; }
	
	float klock();

	int getMouseX();

	int getMouseY();

	FirstPersonCamera& getCamera();

	bool mouseDownLeft();

	bool mouseDownRight();

	bool keystatus(int key);

	bool keyhit(int key);

	void setColor( glm::vec3 c );

	void setColor( float r, float g, float b );

	void setAlpha( float a );

	void setEmissive( glm::vec3 emissive );

	void setOrientation( const glm::mat4 &ori );

	void setScale( float x, float y, float z );

	void setBlend( bool active );

	void moveTo( float x, float y );

	void lineTo( float to_x, float to_y );

	void drawCircle( float x, float y, float radius );

	void drawSphere( glm::vec3 position, float radius );

	void drawCone( glm::vec3 p1, glm::vec3 p2, float radius );

	void drawCube( glm::vec3 position, float radius );

	void drawPlane( glm::vec3 position, glm::vec3 normal, float radius );

	void drawRoundedCube(glm::vec3 pos, float radius, float edge_radius);

	void frame();
private:

	void handle_key(int key, int action);

	static void _key_callback(int key, int action)
	{
		instance->handle_key(key, action);
	}

	static int _closeCallback(void)
	{
		instance->shutdown();
		
		return 0; // Do not close OpenGL context yet...
	}

	void draw_buffered_lines();

	void draw_buffered_circles();

	void draw_buffered_shapes();

	void draw_buffered_objects();

	void init_phong( Shader& active_shader );

	
private:
	static ProtoGraphics *instance;
	bool isRunning;

	int xres, yres;
	int mousx, mousy;

	static const int NUM_KEYS = 325;
	bool key_array[NUM_KEYS];
	int key_hit_array[NUM_KEYS];

	glm::vec4 colorState;
	glm::vec2 move_to_state;
	int blend_state;
	glm::mat4 currentOrientation;
	glm::vec3 scale;

	glm::vec3 light_pos;
	glm::vec3 emissiveColor;

	FirstPersonCamera camera;

	GeometryLibrary geo_lib;
	
	Shader shader_2d;
	Shader shader_lines2d;
	Shader phong_shader;
	Shader geo_shader_normals;
	
	std::vector< LineSegmentState > buffered_lines;
	std::vector< CircleState > buffered_circles;

	std::vector< BaseState3D* > buffered_shapes;

	// Keep track of pointers for deletion. Need to know type to call correct dtor
	std::vector< SphereState* > sphereList;
	std::vector< CylinderState* > cylinderList;
	std::vector< PlaneState* > planeList;
	std::vector< CubeState* > cubeList;
	

	std::vector< Shader* > shader_list;

	float delta;
	float time;
	float old_time;

	unsigned int numframes;

	int num_opaque;
	int num_blended;
};

//};