#pragma comment(lib, "glfw_d.lib")
#pragma comment(lib, "opengl32.lib")



#include "common.h"

#include "camera.h"
#include "shapes/shapes.h"

#include "depends/noise/perlin.h"

#include <vector>
#include <algorithm>


#define M_PI       3.14159265358979323846f
#define TWO_PI     6.28318530717958647692f

template <class T> T DEGREES_TO_RADIANS(T degrees)
{
	return degrees * T(M_PI) / T(180.f);
}

template <class T> T RADIANS_TO_DEGREES(T radians)
{
	return radians / T(M_PI) * T(180.f);
}






class ProtoGraphics
{

public:
	ProtoGraphics()
	{
		instance = this;

		isRunning = false;

		time = 0.f;
		old_time = 0.f;
		delta = 0.f;

		numframes = 0;

		mousx = 0;
		mousy = 0;

		light_pos = glm::vec3( 0.f, 0.f, 0.f );

		for( int i = 0; i < 256; i++ )
		{
			key_array[256] = false;
		}

		blend_state = false;
		colorState.r = 1.f;
		colorState.b = 1.f;
		colorState.g = 1.f;
		colorState.a = 1.f;
		move_to_state = glm::vec2(0.f, 0.f);
	}

	~ProtoGraphics()
	{

	}

	

	bool init(int xres, int yres)
	{
		int ok = glfwInit();
		if ( ok==0 ) return false;
		this->xres = xres;
		this->yres = yres;

		// TODO: use GL 3.3 and core profile that wont run any deprecated stuff
		// TODO will fail if vidcard doesnt support gl 3.3... make a function that tries highest first, then steps down
		// or just remove for release build
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
		ok = glfwOpenWindow(xres,yres,8,8,8,8,24,16,GLFW_WINDOW);
		if (ok == 0 ) return false;


		glfwSetWindowTitle("ProtoWizard script server");
		glfwSwapInterval(1);
		
		glewExperimental = GL_TRUE;
		int err = glewInit();
		if ( GLEW_OK != err )
		{
			printf("Failed to init GLEW\n");
			return false;
		}

		glEnable(GL_MULTISAMPLE);

		glfwSetWindowCloseCallback( &_closeCallback );

		// Clear error caused by GLEW using glGetString instead of glGetStringi( char*, int )
		for ( GLenum Error = glGetError( ); ( GL_NO_ERROR != Error ); Error = glGetError( ) )
		{
		}


		camera.pos = glm::vec3( 0.0f, 0.0f, -50.0f );

		// TODO.. remove this, make it so line is a static class that checks if it
		// has a VBO on draw...
		if ( !line.init() )
		{
			printf("failed to init Line VBO");
			return false;
		}

		if ( !circle.init() )
		{
			printf("failed to init Circle VBO");
			return false;
		}

		if ( !sphere.init() )
		{
			printf("failed to init Sphere VBO");
			return false;
		}

		if ( !cylinder.init() )
		{
			printf("failed to init cylinder VBO");
			return false;
		}

		if ( !cube.init() )
		{
			printf("failed to init cube VBO");
			return false;
		}

		GetError();


		if ( shader_lines2d.install("assets/line2d_shader.vert", "assets/line2d_shader.frag") == false )
		{
			return false;
		}
		shader_list.push_back( &shader_lines2d );

		if ( shader_2d.install("assets/shader2d.vert", "assets/shader2d.frag") == false )
		{
			return false;
		}
		shader_list.push_back( &shader_2d );

		if ( sphere_shader.install("assets/sphere_shader.vert", "assets/sphere_shader.frag") == false )
		{
			return false;
		}
		shader_list.push_back( &sphere_shader );

		if (cylinder_shader.install("assets/drawcone.vert", "assets/drawcone.frag") == false)
		{
			return false;
		}
		shader_list.push_back( &cylinder_shader );

		if (cube_shader.install("assets/pointlight.vert", "assets/pointlight.frag") == false)
		//if (cube_shader.install("assets/cube_shader.vert", "assets/cube_shader.frag") == false)
		{
			return false;
		}
		shader_list.push_back( &cube_shader );

		GetError();
			
		isRunning = true;
		return true;
	}


	void setCamera( glm::vec3 pos, glm::vec3 target, glm::vec3 up )
	//void setCamera( glm::vec3 pos, float horiz, float verti )
	{
		//glm::mat4 rmx = glm::rotate( glm::mat4(1.0), RADIANS_TO_DEGREES<float>(verti), glm::vec3(1.f, 0.f, 0.f)  );
		//glm::mat4 rmy = glm::rotate( glm::mat4(1.0), RADIANS_TO_DEGREES<float>(horiz), glm::vec3(0.f, 1.f, 0.f)  );
		//glm::mat4 rotmat = rmx * rmy;
		//mCam = rotmat;
		//mCam = glm::translate( mCam, pos );

		mCam = glm::lookAt( pos, target, up );
	}

	void shutdown()
	{	
		circle.shutdown();
		line.shutdown();
		sphere.shutdown();
		cube.shutdown();
		cylinder.shutdown();

		for(unsigned int i=0; i<shader_list.size(); i++){
			shader_list[i]->shutdown();
		}

		isRunning = false;

		glfwCloseWindow();
		glfwTerminate();
	}

	void cls( float r, float g, float b )
	{
		assert( isRunning );
		glClearColor( r,  g,  b, 1.0f );
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
	}

	void frame()
	{	
		assert( isRunning );
		GetError();

		time = klock();
		delta = time - old_time;
		old_time = time;

		camera.update( keystatus('A'), keystatus('D'), keystatus('W'), keystatus('S'), (float)getMouseX(), (float)getMouseY(), mouseDown(), 1.0f );


		setColor(1.f, 1.f, 1.f);
		light_pos = glm::vec3( cos(time) * 15.f, sin(time) * 15.f , 0.0f );
		drawSphere( light_pos, 1.0f );

		if ( key_array['R'] )
		{
			key_array['R'] = false;
			printf("/////////////////////////////R E L O A D I N G   S H A D E R S /////////////\n");
			for(unsigned int i=0; i<shader_list.size(); i++)
			{
				if ( shader_list[i]->reload() == true )
				{
					continue;
				}else{
					printf("could not relead shader\n");
					break;
				}
			}
		}

		draw_buffered_objects();

		


		char title_buf[256];
		sprintf_s(title_buf, 256, "%i .... %f", numframes, delta);
		glfwSetWindowTitle(title_buf);

		glfwGetMousePos(&mousx, &mousy);

		numframes++;

		glfwSwapBuffers();

		for(int i=0; i<256; i++)
		{
			glfwGetKey(i) == 1 ? key_array[i]=true : key_array[i]=false;
		}
	}

	bool isWindowOpen()
	{
		return isRunning; //glfwGetWindowParam( GLFW_OPENED ) == 1;
	}

	int getWindowWidth() 
	{
		return xres;
	}

	int getWindowHeight() 
	{
		return yres;
	}

	float klock()
	{
		return (float) glfwGetTime();
	}

	int getMouseX() 
	{
		return mousx;
	}

	int getMouseY() 
	{
		return mousy;
	}

	bool mouseDown()
	{
		return glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) == 1;
	}

	bool keystatus(int key)
	{
		return key_array[key];
	}

	FirstPersonCamera getCamera()
	{
		return camera;
	}


	double octaves_of_noise(int octaves, double x, double y, double z){
		double value = 0.0;
		double vec[3];
		for(int i=0; i<octaves; i++){
			vec[0] = x*pow( 2, (double)i );
			vec[1] = y*pow( 2, (double)i );
			vec[2] = z*pow( 2, (double)i );
			value += noise3( vec );
		}
		return value;
	}

	void setColor( float r, float g, float b )
	{
		colorState.r = r;
		colorState.b = b;
		colorState.g = g;
		colorState.a = 1.0f;
	}

	void setBlend( bool active )
	{
		// TODO guess I want 2D blending at least... so, should each ShapeState have a BLEND_FUNC property?
		// and should blending allways be enabled? yes... maybe... maybe handle it the same way as color
		// so you do setBlend( LIGHT_BLEND ) etc. steal blitzmax' names :]
		// SOLIDBLEND (no blend, overwrite) ALPHABLEND (self explanatory), LIGHTBLEND (additive), SHADEBLEND (multiply with backbuffer, MASKBLEND (draw if alpha > .5 )
		// see http://en.wikibooks.org/wiki/BlitzMax/Modules/Graphics/Max2D#SetBlend
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		blend_state = active;
	}

	void moveTo( float x, float y )
	{
		move_to_state = glm::vec2(x,y);
	}

	void lineTo( float to_x, float to_y )
	{
		LineSegmentState state;
		state.color1 = colorState;
		state.color2 = colorState;
		state.x1 = move_to_state.x;
		state.y1 = move_to_state.y;
		state.x2 = to_x;
		state.y2 = to_y;

		buffered_lines.push_back( state );

		move_to_state = glm::vec2(to_x,to_y); 
	}

	void drawCircle( float x, float y, float radius )
	{
		CircleState state;
		state.color = colorState;
		state.x = x;
		state.y = y;
		state.radius = radius;
		buffered_circles.push_back( state );
	}

	void drawSphere( glm::vec3 position, float radius ) 
	{
		SphereState state;
		state.color = colorState;
		state.x = position.x;
		state.y = position.y;
		state.z = position.z;
		state.radius = radius;
		buffered_spheres.push_back( state );
	}

	void drawCone( glm::vec3 p1, float r1, glm::vec3 p2, float r2 ) 
	{
		CylinderState state;
		state.color = colorState;
		state.p1 = p1;
		state.p2 = p2;
		state.radius1 = r1;
		state.radius2 = r2;
		state.blended = blend_state;
		buffered_cylinders.push_back( state );
	}

	void drawCube( glm::vec3 position, float radius )
	{
		CubeState state;
		state.color = colorState;
		state.x = position.x;
		state.y = position.y;
		state.z = position.z;
		state.radius = radius;
		buffered_cubes.push_back( state );
	}


private:


	static int _closeCallback(void)
	{
		instance->shutdown();
		
		return 0; // Do not close...
	}

	void draw_buffered_lines()
	{		
		shader_lines2d.begin();
		unsigned int loc = shader_lines2d.GetVariable("mvp");
		glm::mat4 orthomat = glm::ortho(  0.f, (float)xres, (float)yres, 0.f );
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(orthomat) );
		line.draw(buffered_lines);
		buffered_lines.clear();
	}

	void draw_buffered_circles()
	{		
		shader_2d.begin();

		// TODO, uniform locs could be moved to init code, so they are only set once, 
		// but since its convenient to modify them, they stay for now.
		unsigned int mvLoc;
		unsigned int projLoc;
		unsigned int colorLoc;
		mvLoc    = shader_2d.GetVariable("modelviewMatrix");
		projLoc  = shader_2d.GetVariable("projMatrix");
		colorLoc = shader_2d.GetVariable("vColor");

		VSML *vsml = VSML::getInstance();
		vsml->initUniformLocs(mvLoc, projLoc);
		// send allready init-ed ortho to shader
		vsml->matrixToUniform(VSML::PROJECTION); 

		for (unsigned int i=0; i<buffered_circles.size(); i++)
		{
			float x = buffered_circles[i].x;
			float y = buffered_circles[i].y;
			float radius = buffered_circles[i].radius;

			ColorRGBA color = buffered_circles[i].color;
			 // Todo. use float vector here? and direct pointer to &color.r
			shader_2d.SetFloat4( colorLoc, color.r, color.g, color.b, color.a );

			vsml->loadIdentity(VSML::MODELVIEW);
			vsml->translate(x, y, 0.0f);
			// Get the scaling factor
			float scale_factor = radius; 
			vsml->scale(scale_factor,scale_factor,scale_factor);

			vsml->matrixToUniform(VSML::MODELVIEW);

			circle.draw();
		}
		buffered_circles.clear();
	}

	void draw_buffered_spheres()
	{
		sphere_shader.begin();

		unsigned int colorLoc = sphere_shader.GetVariable("vColor");
		unsigned int translateLoc = sphere_shader.GetVariable("translate");
		unsigned int radiusLoc = sphere_shader.GetVariable("radius");

		unsigned int projLoc = sphere_shader.GetVariable("projMatrix");
		unsigned int mvLoc = sphere_shader.GetVariable("modelviewMatrix");

		//////////////////////////////////////////////////////////////////////////
		VSML *vsml = VSML::getInstance();
		vsml->initUniformLocs(mvLoc, projLoc);
		vsml->matrixToUniform(VSML::MODELVIEW);
		vsml->matrixToUniform(VSML::PROJECTION);
		//////////////////////////////////////////////////////////////////////////

		for (unsigned int i=0; i<buffered_spheres.size(); i++)
		{
			ColorRGBA color = buffered_spheres[i].color;
			sphere_shader.SetFloat4( colorLoc, color.r, color.g, color.b, color.a );
			sphere_shader.SetFloat( radiusLoc, buffered_spheres[i].radius);
			sphere_shader.SetFloat3( translateLoc, buffered_spheres[i].x, buffered_spheres[i].y, buffered_spheres[i].z );

			sphere.draw();
		}

		buffered_spheres.clear();
	}

	static bool sort_func( const CylinderState &a, const CylinderState &b )
	{
		glm::vec3 cam_pos = instance->getCamera().pos;
		glm::vec3 a_center = a.p1 + 0.5f * (a.p2 - a.p1);
		glm::vec3 b_center = b.p1 + 0.5f * (b.p2 - b.p1);
		float z1 = glm::length( a_center - cam_pos );
		float z2 = glm::length( b_center - cam_pos );
		return z1 < z2;
	}

	void draw_buffered_cylinders()
	{
		// TODO find out how expensive it is to sort opaque and translucent objects into buckets...
		// probably better methods out there, also, these two buckets should only be allocated once
		// instead of alloc/dealloc on each call to draw_buffered....
		std::vector<CylinderState> opaque;
		std::vector<CylinderState> translucent;

		for (unsigned int i=0; i<buffered_cylinders.size(); i++)
		{
			if ( buffered_cylinders[i].blended )
			{
				translucent.push_back( buffered_cylinders[i] );
			}else{
				opaque.push_back( buffered_cylinders[i] );
			}
		}

		cylinder_shader.begin();

		unsigned int colorLoc = cylinder_shader.GetVariable("vColor");
		unsigned int translateLoc = cylinder_shader.GetVariable("translate");

		unsigned int projLoc = cylinder_shader.GetVariable("projMatrix");
		unsigned int mvLoc = cylinder_shader.GetVariable("modelviewMatrix");

		//////////////////////////////////////////////////////////////////////////
		VSML *vsml = VSML::getInstance();
		vsml->initUniformLocs(mvLoc, projLoc);
		vsml->matrixToUniform(VSML::MODELVIEW);
		vsml->matrixToUniform(VSML::PROJECTION);
		//////////////////////////////////////////////////////////////////////////


		// useful article on blending
		// http://blogs.msdn.com/b/shawnhar/archive/2009/02/18/depth-sorting-alpha-blended-objects.aspx

		for (unsigned int i=0; i<opaque.size(); i++)
		{
			ColorRGBA color = opaque[i].color;
			cylinder_shader.SetFloat4( colorLoc, color.r, color.g, color.b, color.a );
			cylinder.draw( opaque[i].p1, opaque[i].radius1, opaque[i].p2, opaque[i].radius2 );
		}

		std::sort( translucent.begin(), translucent.end(), &ProtoGraphics::sort_func );

		if ( translucent.size() > 0 )
		{
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glEnable(GL_BLEND);
		}
		for (unsigned int i=0; i<translucent.size(); i++)
		{
			ColorRGBA color = translucent[i].color;
			cylinder_shader.SetFloat4( colorLoc, color.r, color.g, color.b, color.a );
			cylinder.draw( translucent[i].p1, translucent[i].radius1, translucent[i].p2, translucent[i].radius2 );
		}
		if ( translucent.size() > 0 )
		{
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}

		buffered_cylinders.clear();
	}

	void draw_buffered_cubes()
	{
		cube_shader.begin();

		unsigned int colorLoc = cube_shader.GetVariable("vColor");
		unsigned int translateLoc = cube_shader.GetVariable("translate");
		unsigned int radiusLoc = cube_shader.GetVariable("radius");
		unsigned int lightLoc = cube_shader.GetVariable("light_pos");
		glUniform3f( lightLoc, light_pos.x, light_pos.y, light_pos.z );

		unsigned int projLoc = cube_shader.GetVariable("projMatrix");
		unsigned int mvLoc = cube_shader.GetVariable("modelviewMatrix");

		//////////////////////////////////////////////////////////////////////////
		VSML *vsml = VSML::getInstance();
		vsml->initUniformLocs(mvLoc, projLoc);
		vsml->matrixToUniform(VSML::MODELVIEW);
		vsml->matrixToUniform(VSML::PROJECTION);
		//////////////////////////////////////////////////////////////////////////

		for (unsigned int i=0; i<buffered_cubes.size(); i++)
		{
			ColorRGBA color = buffered_cubes[i].color;
			cube_shader.SetFloat4( colorLoc, color.r, color.g, color.b, color.a );
			cube_shader.SetFloat( radiusLoc, buffered_cubes[i].radius);
			cube_shader.SetFloat3( translateLoc, buffered_cubes[i].x, buffered_cubes[i].y, buffered_cubes[i].z );

			cube.draw();
		}
		buffered_cubes.clear();
	}

	void draw_buffered_objects()
	{
		// Draw 3D first, set up our matrices
		VSML *vsml = VSML::getInstance();

		vsml->loadIdentity(VSML::PROJECTION);
		vsml->perspective(60.0f, xres/(float)yres, 1.0f, 1000.0f);

		glm::mat4 view = camera.getViewMatrix();
		//glm::mat4 view = mCam;
		vsml->loadMatrix( VSML::MODELVIEW, glm::value_ptr(view) );

		// We want depth test enabled for 3D
		glEnable( GL_DEPTH_TEST );

		// turn on culling to make things go a little faster
		glEnable( GL_CULL_FACE );

		// http://www.opengl.org/sdk/docs/man/xhtml/glCullFace.xml
		glCullFace( GL_BACK );
		if ( buffered_spheres.size() > 0 ) 
			draw_buffered_spheres();

		if ( buffered_cubes.size() > 0 ) 
			draw_buffered_cubes();

		// we want to inside cylinders, so don't cull
		glDisable( GL_CULL_FACE );

		if ( buffered_cylinders.size() > 0 ) 
			draw_buffered_cylinders();

		// for 2D we want overdraw in all cases, so turn depth test off
		glDisable( GL_DEPTH_TEST );

		// Setup projection for 2D, but now modelview, as we will modify 
		// modelview in most 2D drawcalls using translation, rotation and scaling.
		vsml->loadIdentity(VSML::PROJECTION);
		vsml->ortho(0.0f, (float)xres, (float)yres, 0.f, -1.f , 1.f);

		if ( buffered_lines.size() > 0 ) 
			draw_buffered_lines();

		if ( buffered_circles.size() > 0 ) 
			draw_buffered_circles();
	}






private:
	static ProtoGraphics *instance;
	bool isRunning;

	int xres, yres;
	int mousx, mousy;
	bool key_array[256];

	ColorRGBA colorState;
	glm::vec2 move_to_state;
	bool blend_state;

	glm::vec3 light_pos;

	FirstPersonCamera camera;
	glm::mat4 mCam;

	Line line;
	Circle circle;
	Cylinder cylinder;
	Sphere sphere;
	Cube cube;
	
	Shader shader_2d;
	Shader shader_lines2d;
	Shader sphere_shader;
	Shader cylinder_shader;
	Shader cube_shader;
	
	std::vector< LineSegmentState > buffered_lines;
	std::vector< CircleState > buffered_circles;
	std::vector< SphereState > buffered_spheres;
	std::vector< CylinderState > buffered_cylinders;
	std::vector< CubeState > buffered_cubes;

	std::vector< Shader* > shader_list;

	float delta;
	float time;
	float old_time;

	unsigned int numframes;
};