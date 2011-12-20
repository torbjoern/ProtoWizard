

#include "common.h"

#include "camera.h"
#include "shapes/shapes.h"

#include "depends/noise/perlin.h"

#include <vector>
#include <algorithm>
#include <functional>




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
private:
	// Disallowing copying. Please pass protographics about as a const ptr or refrence!
	ProtoGraphics(const ProtoGraphics&); // no implementation 
	ProtoGraphics& operator=(const ProtoGraphics&); // no implementation 


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

		for( int i = 0; i < NUM_KEYS; i++ )
		{
			key_array[i] = false;
			key_hit_array[i] = 0;
		}

		blend_state = blending::SOLID_BLEND;
		colorState = glm::vec4( 1.f );
		emissiveColor = glm::vec3( 0.f );
		move_to_state = glm::vec2(0.f, 0.f);

		currentOrientation = glm::mat4( 1.0f );
	}

	~ProtoGraphics()
	{

	}

	// fast float random in interval -1,1
	// source by RGBA: http://www.rgba.org/articles/sfrand/sfrand.htm
	float sfrand( void )
	{
		unsigned int a=(rand()<<16)|rand();  //we use the bottom 23 bits of the int, so one
		//16 bit rand() won't cut it.
		a=(a&0x007fffff) | 0x40000000;  

		return( *((float*)&a) - 3.0f );
	}

	float getMSPF(){
		return delta;
	}

	void dump_stats()
	{
		printf("------------------------\n");
		printf("num opaque: %d\n", num_opaque);
		printf("num blended: %d\n", num_blended);
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
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
		glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
		ok = glfwOpenWindow(xres,yres,8,8,8,8,24,8,GLFW_WINDOW); // Important to either use 32 bit depth, or 24d8s
		if (ok == 0 ) return false;


		glfwSetWindowTitle("ProtoWizard script server");
		glfwSwapInterval(0);
		
		glewExperimental = GL_TRUE;
		int err = glewInit();
		if ( GLEW_OK != err )
		{
			printf("Failed to init GLEW\n");
			return false;
		}

		glfwSetWindowCloseCallback( &_closeCallback );
		glfwSetKeyCallback( &_key_callback );

		// Clear error caused by GLEW using glGetString instead of glGetStringi( char*, int )
		for ( GLenum Error = glGetError( ); ( GL_NO_ERROR != Error ); Error = glGetError( ) )
		{
		}


		geo_lib.init();


		if ( shader_lines2d.install("assets/line2d_shader.vert", "assets/line2d_shader.frag") == false )
		{
			return false;
		}else{
			shader_list.push_back( &shader_lines2d );
		}

		if ( shader_2d.install("assets/shader2d.vert", "assets/shader2d.frag") == false )
		{
			return false;
		}
		shader_list.push_back( &shader_2d );

		if ( phong_shader.install("assets/phong.vert", "assets/phong.frag") == false )
		{
			return false;
		}else{
			shader_list.push_back( &phong_shader );
		}
		

		glEnable(GL_MULTISAMPLE);
		GetError();
			
		isRunning = true;
		return true;
	}


	void setCamera( glm::vec3 pos, glm::vec3 target, glm::vec3 up )
	{
		throw char("not impl");
		//mCam = glm::lookAt( pos, target, up );
	}

	void setCamera( float x, float y, float z, float hang, float vang )
	{
		camera.set( glm::vec3(x,y,z), hang, vang );
	}

	void setTitle( const std::string &str){
		glfwSetWindowTitle(str.c_str() );
	}

	void shutdown()
	{	
		geo_lib.shutdown();

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


		//camera.update( keystatus('A'), keystatus('D'), keystatus('W'), keystatus('S'), (float)getMouseX(), (float)getMouseY(), mouseDownLeft(), delta );


		setColor(1.f, 1.f, 1.f);
		light_pos = glm::vec3( cos(time) * 15.f, 3.0f , sin(time) * 15.f );
		setEmissive( glm::vec3( 1.f ) );
		drawSphere( light_pos, 1.0f );
		setEmissive( glm::vec3( 0.f ) );


		if ( keyhit('R') )
		{
			printf("/////////////////////////////R E L O A D I N G   S H A D E R S /////////////\n");
			for(unsigned int i=0; i<shader_list.size(); i++)
			{
				if ( shader_list[i]->reload() == true )
				{
					continue;
				}else{
					printf("could not reload shader\n");
					break;
				}
			}
		}

		if ( keyhit('1') )
		{
			static bool wire_frame_mode = false;
			wire_frame_mode = !wire_frame_mode;
			
			glPolygonMode(GL_FRONT_AND_BACK, wire_frame_mode ? GL_LINE : GL_FILL);
			
		}
		draw_buffered_objects();

		//char title_buf[256];
		//sprintf_s(title_buf, 256, "%i .... %.1f mspf alpha = %f", numframes, delta*1000.0f, colorState.a);
		//glfwSetWindowTitle(title_buf);

		glfwGetMousePos(&mousx, &mousy);

		numframes++;

		glfwSwapBuffers();
	}

	bool isWindowOpen()
	{
		return isRunning;
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

	bool mouseDownLeft()
	{
		return glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) == 1;
	}

	bool mouseDownRight()
	{
		return glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT ) == 1;
	}

	bool keystatus(int key)
	{
		return key_array[key];
	}

	bool keyhit(int key)
	{
		int times_hit = key_hit_array[key];
		key_hit_array[key] = 0;
		return times_hit > 0;
	}

	FirstPersonCamera& getCamera()
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

	void setColor( glm::vec3 c )
	{
		colorState.x = c.x;
		colorState.y = c.y;
		colorState.z = c.z;
	}

	void setColor( float r, float g, float b )
	{
		colorState.r = r;
		colorState.b = b;
		colorState.g = g;
	}

	void setAlpha( float a )
	{
		colorState.a = a;
	}

	void setEmissive( glm::vec3 emissive )
	{
		emissiveColor = emissive;
	}

	void setOrientation( const glm::mat4 &ori ){
		currentOrientation = ori;
	}

	void setBlend( bool active )
	{
		// TODO guess I want 2D blending at least... so, should each ShapeState have a BLEND_FUNC property?
		// and should blending allways be enabled? yes... maybe... maybe handle it the same way as color
		// so you do setBlend( LIGHT_BLEND ) etc. steal blitzmax' names :]
		// SOLIDBLEND (no blend, overwrite) ALPHABLEND (use alpha channel in image and specified draw color RGBA), LIGHTBLEND (additive), SHADEBLEND (multiply with backbuffer, MASKBLEND (draw if alpha > .5 )
		// see http://en.wikibooks.org/wiki/BlitzMax/Modules/Graphics/Max2D#SetBlend
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		if ( active )
		{
			blend_state = blending::ADDITIVE_BLEND;
		}else{
			blend_state = blending::SOLID_BLEND;
		}
		
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
		SphereState *state = new SphereState;
		state->color = colorState;
		state->emissiveColor = emissiveColor;
		state->transform = glm::translate( glm::mat4( 1.0f ), position );
		state->radius = radius;
		state->blend_mode = blend_state;
		buffered_shapes.push_back( state );
	}

	void drawCone( glm::vec3 p1, glm::vec3 p2, float radius ) 
	{
		CylinderState *state = new CylinderState;
		state->color = colorState;
		state->emissiveColor = emissiveColor;
		state->p1 = p1;
		state->p2 = p2;
		state->radius = radius;
		state->blend_mode = blend_state;
		buffered_shapes.push_back( state );
	}

	void drawCube( glm::vec3 position, float radius )
	{
		CubeState *state = new CubeState;
		state->color = colorState;
		state->emissiveColor = emissiveColor;
		state->transform = glm::translate( glm::mat4(1.0), position );
		state->transform *= currentOrientation;
		
		
		state->radius = radius;
		state->blend_mode = blend_state;
		buffered_shapes.push_back( state );
	}

	void drawPlane( glm::vec3 position, glm::vec3 normal, float radius )
	{
		PlaneState *state = new PlaneState;
		state->color = colorState;
		state->emissiveColor = emissiveColor;
		state->transform = glm::translate( glm::mat4( 1.0f ), position );
		state->normal = normal;
		state->radius = radius;
		state->blend_mode = blend_state;
		buffered_shapes.push_back( state );
	}

	void drawRoundedCube(float radius, float edge_radius)
	{
		float sc = radius;
		glm::vec3 v0(-sc,-sc,-sc);
		glm::vec3 v1(-sc,-sc,+sc);
		glm::vec3 v2(-sc,+sc,-sc);
		glm::vec3 v3(-sc,+sc,+sc);

		glm::vec3 v4(+sc,-sc,-sc);
		glm::vec3 v5(+sc,-sc,+sc);
		glm::vec3 v6(+sc,+sc,-sc);
		glm::vec3 v7(+sc,+sc,+sc);

#define drawline(v1,v2) (drawCone(v1,v2,edge_radius))
		// bottom quad
		drawline(v0,v1);
		drawline(v0,v4);
		drawline(v4,v5);
		drawline(v1,v5);
		// top quad
		drawline(v2,v3);
		drawline(v2,v6);
		drawline(v6,v7);
		drawline(v7,v3);

		// connectors top/bottom quad
		drawline(v0,v2);
		drawline(v4,v6);
		drawline(v1,v3);
		drawline(v7,v5);
#undef drawline

		drawSphere(v0, edge_radius);
		drawSphere(v1, edge_radius);
		drawSphere(v2, edge_radius);
		drawSphere(v3, edge_radius);
		drawSphere(v4, edge_radius);
		drawSphere(v5, edge_radius);
		drawSphere(v6, edge_radius);
		drawSphere(v7, edge_radius);

		glm::vec3 n0 ( 0.f, 0.f, -1.f);
		glm::vec3 n1 ( 0.f, 0.f, +1.f);
		glm::vec3 n2 ( 0.f, -1.f, 0.f);
		glm::vec3 n3 ( 0.f, +1.f, 0.f);
		glm::vec3 n4 ( -1.f, 0.f, 0.f);
		glm::vec3 n5 ( +1.f, 0.f, 0.f);
		
		float radius2 = radius + edge_radius;
		drawPlane(n0*radius2,n0, radius );
		drawPlane(n1*radius2,n1, radius );
		drawPlane(n2*radius2,n2, radius );
		drawPlane(n3*radius2,n3, radius );
		drawPlane(n4*radius2,n4, radius );
		drawPlane(n5*radius2,n5, radius );

	}


private:

	void handle_key(int key, int action)
	{		
		key_array[key] = (action == GLFW_PRESS);

		if( (action == GLFW_PRESS) )
		{
			key_hit_array[key]++;
		}
	}


	static void _key_callback(int key, int action)
	{
		instance->handle_key(key, action);
	}

	static int _closeCallback(void)
	{
		instance->shutdown();
		
		return 0; // Do not close OpenGL context yet...
	}

	void draw_buffered_lines()
	{		
		shader_lines2d.begin();
		unsigned int loc = shader_lines2d.GetVariable("mvp");
		glm::mat4 orthomat = glm::ortho(  0.f, (float)xres, (float)yres, 0.f );
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(orthomat) );
		geo_lib.line.draw(buffered_lines);
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

			glm::vec4 color = buffered_circles[i].color;
			 // Todo. use float vector here? and direct pointer to &color.r
			shader_2d.SetFloat4( colorLoc, color.r, color.g, color.b, color.a );

			vsml->loadIdentity(VSML::MODELVIEW);
			vsml->translate(x, y, 0.0f);
			// Get the scaling factor
			float scale_factor = radius; 
			vsml->scale(scale_factor,scale_factor,scale_factor);

			vsml->matrixToUniform(VSML::MODELVIEW);

			geo_lib.circle.draw();
		}
		buffered_circles.clear();
	}

	void _init_phong()
	{
		unsigned int loc1 = phong_shader.GetVariable("lightIntensity");
		unsigned int loc2 = phong_shader.GetVariable("ambientIntensity");

		phong_shader.SetFloat4( loc1, 0.8f, 0.8f, 0.8f, 1.0f );
		phong_shader.SetFloat4( loc2, 0.1f, 0.1f, 0.1f, 1.0f );

		phong_shader.SetVec3( phong_shader.GetVariable("cameraSpaceLightPos"), light_pos );
		phong_shader.SetFloat( phong_shader.GetVariable("lightAttenuation"), 0.02f );
		phong_shader.SetFloat( phong_shader.GetVariable("shininess"), 10.0f );
	}


	void draw_buffered_shapes()
	{
		// TODO find out how expensive it is to sort opaque and translucent objects into buckets...
		// probably better methods out there, also, these two buckets should only be allocated once
		// instead of alloc/dealloc on each call to draw_buffered....
		std::vector<BaseState3D*> opaque;
		std::vector<BaseState3D*> translucent;

		for (unsigned int i=0; i<buffered_shapes.size(); i++)
		{
			if ( buffered_shapes[i]->blend_mode != blending::SOLID_BLEND )
			{
				translucent.push_back( buffered_shapes[i] );
			}else{
				opaque.push_back( buffered_shapes[i] );
			}
		}

		num_opaque = opaque.size();
		num_blended = translucent.size();

		//////////////////////////////////////////////////////////////////////////
		phong_shader.begin();
		_init_phong();

		unsigned int worldLoc = phong_shader.GetVariable("worldMatrix");
		unsigned int viewLoc = phong_shader.GetVariable("viewMatrix");
		unsigned int projLoc = phong_shader.GetVariable("projMatrix");
		
		VSML *vsml = VSML::getInstance();
		//vsml->initUniformLocs(mvLoc, projLoc);
		//vsml->matrixToUniform(VSML::PROJECTION);

		//uniform mat4 worldMatrix;
		//uniform mat4 viewMatrix;
		//uniform mat4 projMatrix;

		glm::mat4 projection =
			glm::perspective(60.0f, xres/(float)yres, 1.0f, 1000.f);
		glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr(projection) );

		glm::mat4 viewMatrix = camera.getViewMatrix();
		glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix) );
		//vsml->loadMatrix( VSML::PROJECTION, glm::value_ptr(projection) );
		

		unsigned int colorLoc = phong_shader.GetVariable("diffuseColor");
		//////////////////////////////////////////////////////////////////////////


		// useful article on blending
		// http://blogs.msdn.com/b/shawnhar/archive/2009/02/18/depth-sorting-alpha-blended-objects.aspx

		for (unsigned int i=0; i<opaque.size(); i++)
		{
				BaseState3D *state = opaque[i];
				phong_shader.SetVec4( colorLoc, state->color );
				state->pre_draw( phong_shader );
				state->draw(&geo_lib);	
		}

		struct SortFunctor 
		{
			SortFunctor( const FirstPersonCamera &cam ) : camera( cam )
			{
			}

			// std::sort expects a test like a < b for ascending sort order
			bool operator() ( BaseState3D* a, BaseState3D* b )
			{
				return a->distance_from_camera(camera.getPos() ) > b->distance_from_camera(camera.getPos() ); // the one furthest away is to be drawn first.
			}


			FirstPersonCamera camera;
		};

		bool have_translucent_objs = translucent.size() > 0;

		if ( have_translucent_objs )
		{
			std::sort( translucent.begin(), translucent.end(), SortFunctor(camera) );

			// http://www.opengl.org/sdk/docs/man/xhtml/glDepthMask.xml
			glDepthMask(GL_FALSE);

			//http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Don't care about alpha of what we write over
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blending incoming and inversely what was there before
			glEnable(GL_BLEND);


			int num_objs = translucent.size();
			//for (int i=0; i<num_objs; i++)
			for (unsigned int i=num_objs-1; i>0; i--)
			{
				BaseState3D *state = translucent[i];
				phong_shader.SetVec4( colorLoc, state->color );
				state->pre_draw( phong_shader );
				translucent[i]->draw( &geo_lib );
			}

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}



		opaque.clear();
		translucent.clear();


		for(unsigned i=0; i<buffered_shapes.size(); i++)
		{
			delete buffered_shapes[i];
		}

		buffered_shapes.clear();
	}

	void draw_buffered_objects()
	{
		// Draw 3D first

		// We want depth test enabled for 3D
		glEnable( GL_DEPTH_TEST );

		// turn on culling to make things go a little faster
		glEnable( GL_CULL_FACE );

		// http://www.opengl.org/sdk/docs/man/xhtml/glCullFace.xml
		glCullFace( GL_BACK );

		if ( buffered_shapes.size() > 0 ) 
			draw_buffered_shapes();

		// for 2D we want overdraw in all cases, so turn depth test off
		glDisable( GL_DEPTH_TEST );

		// Setup projection for 2D, but now modelview, as we will modify 
		// modelview in most 2D drawcalls using translation, rotation and scaling.
		VSML *vsml = VSML::getInstance();
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

	static const int NUM_KEYS = 325;
	bool key_array[NUM_KEYS];
	int key_hit_array[NUM_KEYS];

	glm::vec4 colorState;
	glm::vec2 move_to_state;
	int blend_state;
	glm::mat4 currentOrientation;

	glm::vec3 light_pos;
	glm::vec3 emissiveColor;

	FirstPersonCamera camera;

	GeometryLibrary geo_lib;
	
	Shader shader_2d;
	Shader shader_lines2d;
	Shader phong_shader;
	
	std::vector< LineSegmentState > buffered_lines;
	std::vector< CircleState > buffered_circles;

	std::vector< BaseState3D* > buffered_shapes;

	std::vector< Shader* > shader_list;

	float delta;
	float time;
	float old_time;

	unsigned int numframes;

	int num_opaque;
	int num_blended;
};