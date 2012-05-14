
#include "proto/protographics.h"

using namespace protowizard;

#include "proto/camera.h"
#include "proto/shapes/shapes.h"

#include "proto/texture_manager.h"
#include "proto/mesh_manager.h"
#include "proto/path.h"

#include "proto/camera.h"
#include "proto/shader.h"

#include "proto/math/math_ray.h"
#include "proto/math/math_common.h"
#include "proto/opengl_stuff.h"

#include <vector>
#include <algorithm>
#include <functional>

#define GLM_SWIZZLE 
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp> // Value ptr -> column-ordered pointer to GLM type


#include <GL/glfw.h>


class ProtoGraphicsImplementation : public ProtoGraphics
{
private:
	typedef std::shared_ptr<BaseState3D> BaseState3DPtr;
private:
	// Disallowing copying. Please pass protographics about as a const ptr or refrence!
	//ProtoGraphicsImplementation(const ProtoGraphicsImplementation&); // no implementation 
	//ProtoGraphicsImplementation& operator=(const ProtoGraphicsImplementation&); // no implementation 

public:

	~ProtoGraphicsImplementation() {
		shutdown();
	}

	virtual bool init(int xres, int yres, const std::string resDir )
	{
		setResourceDir( resDir );

		texture_manager = TextureManager::init();
		mesh_manager = MeshManager::init();
		camera = new FirstPersonCamera;

		printf("ProtoGraphicsImplementation v 0.1\n");
		instance = this;

		hasShutdown = false;
		isRunning = false;

		time = 0.0;
		old_time = 0.0;
		delta_time = 0.0;

		max_millis_per_frame = 1.0 / 1000.0; // cap framerate at 1000 fps

		numframes = 0;

		mousx = 0;
		mousy = 0;

		light_pos = glm::vec3( -1000.f, 1000.f, -1000.f );

		for( int i = 0; i < NUM_KEYS; i++ )
		{
			key_array[i] = false;
			key_hit_array[i] = 0;
		}

		blend_state = blending::SOLID_BLEND;
		colorState = glm::vec4( 1.f );
		move_to_state = glm::vec2(0.f, 0.f);

		currentOrientation = identityMatrix;
		scale = glm::vec3( 1.0f );

		isDebugNormalsActive = false;

		currentSample = 0;
		for(int i=0; i<10; i++) {
			mspf_samples[i] = 0;
		}

		int ok = glfwInit();
		if ( ok==0 ) return false;
		this->xres = xres;
		this->yres = yres;

		// TODO: use GL 3.3 and core profile that wont run any deprecated stuff
		// TODO will fail if vidcard doesnt support gl 3.3... make a function that tries highest first, then steps down
		// or just remove for release build
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); // was using 3.2, but soil isn't compatible
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
		//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // requires gl 3.2?
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
		ok = glfwOpenWindow(xres,yres,8,8,8,8,24,8,GLFW_WINDOW); // Important to either use 32 bit depth, or 24d8s
		if (ok == 0 ) return false;

		glfwSetWindowTitle("ProtoWizard project");
		glfwSwapInterval(0); // vsync on/off
		
		if ( gl3wInit() ) {
			fprintf(stderr, "failed to initialize OpenGL\n");
			return false;
		}
		if (!gl3wIsSupported(3, 0)) {
			fprintf(stderr, "OpenGL 3.0 not supported\n");
			return false;
		}
		//printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		//       glGetString(GL_SHADING_LANGUAGE_VERSION));

		glfwSetWindowCloseCallback( &_closeCallback );
		glfwSetKeyCallback( &_key_callback );
		glfwSetMouseButtonCallback( &_key_callback );

		// Clear error caused by GLEW using glGetString instead of glGetStringi( char*, int )
		for ( GLenum Error = glGetError( ); ( GL_NO_ERROR != Error ); Error = glGetError( ) )
		{
		}


		Shapes.init();

		if ( !install_shaders() ) return false;
		
		glEnable(GL_MULTISAMPLE);			
		isRunning = true;

	#ifdef _DEBUG
		 GetError("ProtoGraphicsImplementation::init"); 
	#endif
		return true;
	}


	virtual float getMSPF()	
	{
			return (float)delta_time;
	}

	virtual float getAverageMSPF()
	{
		double acc = 0.f;
		for (int i=0; i<10; i++) {
			acc += mspf_samples[i];
		}
		return (float) acc / 10.f;
	}

	virtual void debugNormals( bool enable )
	{
		isDebugNormalsActive = enable;
	}

	virtual bool isWindowOpen() 
	{
		return isRunning;
	}

	virtual int getWindowWidth() 
	{
		return xres;
	}

	virtual int getWindowHeight() 
	{
		return yres;
	}

	virtual double klock()
	{
		return glfwGetTime();
	}

	virtual float getNormalizedMouseX() 
	{
		return mousx / (float)getWindowWidth();
	}

	virtual float getNormalizedMouseY() 
	{
		return mousy / (float)getWindowHeight();
	}

	virtual int getMouseX() 
	{
		return mousx;
	}

	virtual int getMouseY() 
	{
		return mousy;
	}

	virtual int getMouseWheel() 
	{
		return glfwGetMouseWheel();
	}

	virtual bool mouseDownLeft()
	{
		return glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) == 1;
	}

	virtual bool mouseDownRight()
	{
		return glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT ) == 1;
	}

	virtual bool keystatus(int key)
	{
		return key_array[key];
	}

	virtual bool keyhit(int key)
	{
		int times_hit = key_hit_array[key];
		key_hit_array[key] = 0;
		return times_hit > 0;
	}

	virtual Ray getMousePickRay()
	{
		Ray ray;
		ray.origin = camera->getPos();
		float cameraPlaneDistance = 1.0f / (2.0f * tan(camera->getFov()*M_PI/180.f*0.5f) );
		float u = -.5f + (mousx+.5f) / (float)(xres-1);
		float aspect = xres / float(yres);
		u *= aspect;

		float v = -.5f + (mousy+.5f) / (float)(yres-1);
		ray.dir = camera->getLookDirection() * cameraPlaneDistance + camera->getStrafeDirection() * u + camera->getUpDirection() * -v;
		ray.dir = glm::normalize(ray.dir);
		return ray;
	}

	virtual FirstPersonCamera* getCamera()
	{
		return camera;
	}

	virtual void clz()
	{
		assert( isRunning );
		glClear(GL_DEPTH_BUFFER_BIT );
	}

	virtual void cls( float r, float g, float b )
	{
		assert( isRunning );
		glClearColor( r,  g,  b, 1.0f );
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
	#ifdef _DEBUG
		GetError("ProtoGraphicsImplementation::cls");
	#endif
	}

	virtual void setFrameRate( int frames_per_second )
	{
		if ( frames_per_second >= 1 )
		{
			max_millis_per_frame = 1.0 / double(frames_per_second);
		}
	}

	virtual void setTitle( const std::string &str)
	{
		glfwSetWindowTitle( str.c_str() );
	}
	
	virtual std::string getResourceDir() { return resource_dir; }

	virtual void setResourceDir( const std::string& path ) { resource_dir = path; }

	virtual void moveTo( float x, float y )
	{
		move_to_state = glm::vec2(x,y);
	}

	virtual void lineTo( float to_x, float to_y )
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

	virtual void drawCircle( float x, float y, float radius )
	{
		buffered_circles.push_back( CircleState(colorState,blend_state,x,y,radius) );
	}

	virtual void drawSphere( glm::vec3 position, float radius ) 
	{
		auto state = std::make_shared<SphereState>();
		save_state( state, get3DTransform(currentOrientation, position, glm::vec3(radius) ) );
	}

	// Create a matrix that will orient an object from p1 to p2
	// currently for cylinders & rods
	// could be extended with scale along x,y,z indep....
	glm::mat4 calcMatrix(const glm::vec3& p1, const glm::vec3& p2, float radius)
	{
		glm::vec3 normal = glm::normalize(p2 - p1);
		glm::vec3 not_normal = normal;

		glm::vec3 perp = normal;
		float eps = 1e-7f;
		if ( fabs(not_normal.x) < eps && fabs(not_normal.z) < eps){ // comparing to eps instead of bla == 0
			not_normal.x += 1.0f;
		}else{
			not_normal.y += 1.0f;
		}

		glm::vec3 a = glm::normalize( glm::cross(perp,not_normal) );
		glm::vec3 b = glm::cross(perp,a);

		float length = glm::distance( p1, p2 );
		return glm::mat4( glm::vec4(radius*a, 0.f),		// X-axis
						  glm::vec4(length*normal, 0.f),// Y-axis
						  glm::vec4(radius*b, 0.f),		// Z-axis
						  glm::vec4(p1,1.f) );			// Position
	}

	virtual void drawCone( glm::vec3 p1, glm::vec3 p2, float radius ) 
	{
		auto state = std::make_shared<CylinderState>();
		//glm::vec3 pos = 0.5f*(p1+p2);
		//const glm::mat4& worldTf = get3DTransform(currentOrientation, pos, scale );
		const glm::mat4& localTf = calcMatrix( p1, p2, radius );
		save_state( state, localTf );
		state->hasCap = radius >= 0; // if neg radius, dont draw cap
	}

	virtual void drawCube( glm::vec3 position )
	{
		auto state = std::make_shared<CubeState>();
		save_state( state, get3DTransform(currentOrientation, position, scale) );
	}

	virtual void drawPlane( glm::vec3 position, glm::vec3 normal, float radius )
	{
		auto state = std::make_shared<PlaneState>();
		save_state( state, get3DTransform(currentOrientation, position, glm::vec3(radius) ) );
		state->normal = normal;
	}

	virtual void drawMesh( glm::vec3 position, float horiz_ang, float verti_ang, std::string path )
	{
		// backup current tfrom matrix state
		glm::mat4 backup = currentOrientation;
	
		// create a rotation matrix given horiz- and verti angles
		glm::mat4 xrot = glm::rotate( identityMatrix, verti_ang, glm::vec3(1.f, 0.f, 0.f) );
		glm::mat4 yrot = glm::rotate( identityMatrix, horiz_ang, glm::vec3(0.f, 1.f, 0.f) );
		currentOrientation = glm::transpose(xrot * yrot);

		// draw, and restore current tform
		drawMesh( position, path );
		currentOrientation = backup;
	}

	virtual void drawMesh( glm::vec3 position, std::string path )
	{
		std::shared_ptr<MeshState> state = std::make_shared<MeshState>();
		save_state( state, get3DTransform(currentOrientation, position, scale ));
		state->isTwoSided = false;
		state->mesh = mesh_manager->getMesh(path);
	}

	virtual void drawMesh( MeshPtr mesh, bool isTwoSided )
	{
		std::shared_ptr<MeshState> state = std::make_shared<MeshState>();
		save_state( state, get3DTransform(currentOrientation, glm::vec3(0.f), scale ));
		state->isTwoSided = isTwoSided;
		state->mesh = mesh;
	}

	virtual void setOrientation( const glm::mat4 &ori )
	{
		currentOrientation = ori;
	}

	virtual void setScale( float x, float y, float z )
	{
		scale = glm::vec3(x,y,z);
	}

	virtual void setScale( float uniform_scale )
	{
		scale = glm::vec3( uniform_scale );
	}

	virtual void setColor( const glm::vec3 &c )
	{
		colorState.x = c.x; colorState.y = c.y; colorState.z = c.z;
	}

	virtual void setColor( float r, float g, float b )
	{
		colorState.r = r; colorState.b = b; colorState.g = g;
	}

	virtual void setAlpha( float a )
	{
		colorState.a = a;
	}

	virtual void setBlend( bool active )
	{
		if ( active )
		{
			blend_state = blending::ALPHA_BLEND;
		}else{
			blend_state = blending::SOLID_BLEND;
		}
	}

	//virtual void disableBlending() { blend_state = blending::SOLID_BLEND; } // SOLIDBLEND (no blend, overwrite)
	//virtual void setAlphaBlend() { blend_state = blending::SOLID_BLEND;		// ALPHABLEND (use alpha channel in image and current color.a (alpha) )
	virtual void setLightBlend() { blend_state = blending::ADDITIVE_BLEND; }// LIGHTBLEND (additive)
	//virtual void setShadeBlend() { blend_state = blending::SHADE_BLEND; }// SHADEBLEND (multiply with backbuffer
	//virtual void setMaskBlend() { blend_state = blending::MASK_BLEND; }// MASKBLEND (draw if alpha > .5 )

	virtual void setTexture( const std::string& path )
	{
		texture_manager->setTexture( path );
	}

	virtual void disableTexture()
	{
		texture_manager->disableTextures();
	}

	virtual void toggleWireframe()
	{
		static bool wire_frame_mode = false;
		wire_frame_mode = !wire_frame_mode;			
		glPolygonMode(GL_FRONT_AND_BACK, wire_frame_mode ? GL_LINE : GL_FILL);		
	}

	virtual void reloadShaders()
	{
		printf("\nR E L O A D I N G   S H A D E R S\n\n");
		for(unsigned int i=0; i<shader_list.size(); i++)
		{
			if ( shader_list[i]->reload() == false )
			{
				printf("could not reload shader\n");
				return;
			}
		}
	}

	virtual void frame()
	{	
		assert( isRunning );
	#ifdef _DEBUG
		GetError("ProtoGraphicsImplementation::frame begin");
	#endif

		drawFrame();

		glfwGetMousePos(&mousx, &mousy);

		double time_since_program_started = glfwGetTime();
		delta_time = time_since_program_started - old_time;
		old_time = time_since_program_started;

		mspf_samples[currentSample] = delta_time;
		currentSample++;
		if ( currentSample > 9 ) {
			currentSample = 0;
		}

		// time program har spent running. minimized state not counted
		time += delta_time;

		double time_to_sleep = 0.0;
		if ( delta_time < max_millis_per_frame ) {
			time_to_sleep = max_millis_per_frame - delta_time;
			if ( time_to_sleep > 0.0 ) {
				glfwSleep( time_to_sleep );
			}
		}

		//printf("sleep %d percent of available frametime\n", (int) (100.0 * delta_time / max_millis_per_frame) );

		//char title_buf[256];
		//sprintf_s(title_buf, 256, " %2.2f mspf, %2.2f mspf with sleep,  numObjs = %i, max = %i", delta_time*1000.0, (delta_time+time_to_sleep)*1000.0, num_objs, max_objs);
		//glfwSetWindowTitle(title_buf);

	#ifdef _DEBUG
		GetError("ProtoGraphicsImplementation::frame end");
	#endif

		numframes++;
		glfwSwapBuffers();
	}
private:
	// shutdown is called by the class destructor
	// C++ classes call their own dtor first, then dtors of their members
	// this means we can't rely on dtors freeing opengl resources, not that it matters really
	// the graphics drivers will clean up any un-released objects anyway once the program has shut down
	void shutdown()
	{	
		if ( hasShutdown ) {
			throw std::runtime_error("tried shutting down twice. should not happen. as only dtor calls shutdown");
		}
		texture_manager->shutdown( texture_manager );
		mesh_manager->shutdown( mesh_manager );
		Shapes.de_init();
		delete camera;

		for( auto it=shader_list.begin(); it != shader_list.end(); ++it )
		{
			(*it)->shutdown();
		}

		glfwCloseWindow();
		glfwTerminate();

		hasShutdown = true;
		isRunning = false;
	}

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
		// dont shut down ProtoGraphics yet. Allow its dtor to run
		instance->isRunning = false;
		
		return 0; // Do not close OpenGL context yet...
	}

	void draw_buffered_lines()
	{		
		shader_lines2d->begin();
		unsigned int loc = shader_lines2d->GetVariable("mvp");
		glm::mat4 orthomat = glm::ortho(  0.f, (float)xres, (float)yres, 0.f );
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(orthomat) );
		Shapes.line.draw(buffered_lines);
		buffered_lines.clear();
	}

	void draw_buffered_circles()
	{		
		glm::mat4 modelview_2d_matrix;

		unsigned int mvLoc; 
		mvLoc = shader_2d->GetVariable("modelviewMatrix");

		unsigned int colorLoc;
		colorLoc = shader_2d->GetVariable("vColor");

		for (unsigned int i=0; i<buffered_circles.size(); i++)
		{
			CircleState& circle = buffered_circles[i];
			if ( circle.blend_mode != blending::SOLID_BLEND )
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE); // GL_ONE dont care, or GL_ONE_MINUS_SRC_ALPHA
			} else {
				glDisable(GL_BLEND);
			}

			float x = buffered_circles[i].x;
			float y = buffered_circles[i].y;
			float radius = fabs(buffered_circles[i].radius);

			glm::vec4 color = buffered_circles[i].color;
			shader_2d->SetVec4( colorLoc, color );

			modelview_2d_matrix = glm::translate( identityMatrix, glm::vec3(x, y, 0.0f) );

			// Scale unit circle geometry by desired radius
			modelview_2d_matrix = glm::scale( modelview_2d_matrix, glm::vec3(radius) );
			glUniformMatrix4fv( mvLoc, 1, GL_FALSE, glm::value_ptr(modelview_2d_matrix) );

			if ( buffered_circles[i].radius < 0.f )
			{
				Shapes.circle.draw_open();
			} 
			else
			{
				Shapes.circle.draw_fill();
			}
		
		}
		buffered_circles.clear();
	}

	void draw3DShapes( const Shader& active_shader_ref )
	{
		// use textures as long as
		bool useTexture = (&active_shader_ref) != geo_shader_normals.get();

		unsigned int worldLoc = active_shader_ref.GetVariable("worldMatrix");
		unsigned int viewLoc = active_shader_ref.GetVariable("viewMatrix");
		unsigned int projLoc = active_shader_ref.GetVariable("projMatrix");
		glm::mat4 projection =
			glm::perspective( camera->getFov(), xres/(float)yres, camera->getNearDist(), camera->getFarDist() );
		glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr(projection) );

		glm::mat4 viewMatrix = camera->getViewMatrix();
		glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix) );

		unsigned int colorLoc = active_shader_ref.GetVariable("diffuseColor");

		//////////////////////////////////////////////////////////////////////////


		// useful article on blending
		// http://blogs.msdn.com/b/shawnhar/archive/2009/02/18/depth-sorting-alpha-blended-objects.aspx
		// also a nice explaination
		// http://www.codermind.com/answers/What-is-order-independent-transparency.html

		for (unsigned int i=0; i<opaque.size(); i++)
		{
				BaseState3DPtr state = opaque[i];
				active_shader_ref.SetVec4( colorLoc, state->color );
				state->pre_draw( active_shader_ref, useTexture );
				state->draw();	
		}


		bool have_translucent_objs = translucent.size() > 0;

		if ( have_translucent_objs )
		{
			// http://www.opengl.org/sdk/docs/man/xhtml/glDepthMask.xml
			glDepthMask(GL_FALSE);

			glEnable(GL_BLEND);
			int num_objs = translucent.size();
			for (int i=0; i<num_objs; i++)
			{
				BaseState3DPtr state = translucent[i];
				active_shader_ref.SetVec4( colorLoc, state->color );
				state->pre_draw( active_shader_ref, useTexture );
				translucent[i]->draw();
			}

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}

	#ifdef _DEBUG
		 GetError("ProtoGraphicsImplementation::draw3DShapes 3D end"); 
	#endif
	}

	void drawFrame()
	{
		// Draw 3D first

		// We want depth test enabled for 3D
		glEnable( GL_DEPTH_TEST );

		// turn on culling to make things go a little faster
		glEnable( GL_CULL_FACE );

		// http://www.opengl.org/sdk/docs/man/xhtml/glCullFace.xml
		glCullFace( GL_BACK );

		num_opaque = opaque.size();
		num_blended = translucent.size();
		if ( num_opaque > 0 || num_blended > 0 )
		{
			if ( isDebugNormalsActive ) {
				geo_shader_normals->begin();
				draw3DShapes( (*geo_shader_normals) );
			}

			phong_shader->begin();
			init_phong( (*phong_shader) );
			draw3DShapes( (*phong_shader) );
		

			opaque.clear();
			translucent.clear();

		}

		// for 2D we want overdraw in all cases, so turn depth test off
		glDisable( GL_DEPTH_TEST );


		if ( buffered_lines.size() > 0 ) 
			draw_buffered_lines();

		shader_2d->begin();

		// TODO, uniform locs could be moved to init code, so they are only set once, 
		// but since its convenient to modify them, they stay for now.
		unsigned int mvLoc;
		unsigned int projLoc;
		mvLoc    = shader_2d->GetVariable("modelviewMatrix");
		projLoc  = shader_2d->GetVariable("projMatrix");
	
		// Setup projection for 2D, but now modelview, as we will modify 
		// modelview in most 2D drawcalls using translation, rotation and scaling.
		glm::mat4 ortho_perspective_matrix = glm::ortho(0.0f, (float)xres, (float)yres, 0.f, -1.f , 1.f);
		glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr(ortho_perspective_matrix) );

		if ( buffered_circles.size() > 0 ) 
			draw_buffered_circles();

	#ifdef _DEBUG
		 GetError("ProtoGraphicsImplementation::draw3DShapes 2D end"); 
	#endif
	}

	void init_phong( const Shader& active_shader )
	{
		active_shader.SetVec3( active_shader.GetVariable("cameraSpaceLightPos"), light_pos );

		int tex0 = glGetUniformLocation( active_shader.getProgram() , "tex0");
		glUniform1i(tex0, 0); // sampler "tex0" refers to texture unit 0
	}

	bool install_shaders()
	{
		shader_lines2d = std::make_shared<Shader>( resource_dir+"line2d_shader.vert", resource_dir+"line2d_shader.frag" );
		shader_2d = std::make_shared<Shader>( resource_dir+"shader2d.vert", resource_dir+"shader2d.frag" );
		phong_shader = std::make_shared<Shader>(resource_dir+"phong.vert", resource_dir+"phong.frag");
		geo_shader_normals = std::make_shared<Shader>(resource_dir+"passthru.vert", resource_dir+"normals.gs", resource_dir+"normals.frag");

		shader_list.push_back( shader_lines2d );
		shader_list.push_back( shader_2d );
		shader_list.push_back( phong_shader );
		shader_list.push_back( geo_shader_normals );
		for( auto it=shader_list.begin(); it != shader_list.end(); ++it )
		{
			if ( (*it)->load() == false ) {
				return false;
			}
		}

		int tex0 = glGetUniformLocation( phong_shader->getProgram() , "tex0");
		phong_shader->begin();
		glUniform1i(tex0, 0); // sampler "tex0" refers to texture unit 0

	#ifdef _DEBUG
		 GetError("ProtoGraphicsImplementation::install_shaders end"); 
	#endif

		return true;
	}

	glm::mat4 get3DTransform(const glm::mat4& orientation, const glm::vec3& position, const glm::vec3 scale )
	{
		glm::mat4 transform = glm::translate( identityMatrix, position );
		transform *= orientation;
		transform = glm::scale( transform, scale );
		return transform;
	}
	
	void save_state( BaseState3DPtr state, const glm::mat4& transform )
	{
		state->color = this->colorState;
		state->blend_mode = this->blend_state;
		state->tex_handle = texture_manager->getActiveTexture();
		state->transform = transform;

		if( blend_state == blending::SOLID_BLEND )
		{
			opaque.push_back( state );
		}else{
			translucent.push_back(state);
		}
	}

private:
	static ProtoGraphicsImplementation *instance;
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
	MeshManager *mesh_manager;
	
	typedef std::shared_ptr<Shader> ShaderPtr;
	ShaderPtr shader_2d;
	ShaderPtr shader_lines2d;
	ShaderPtr phong_shader;
	ShaderPtr geo_shader_normals;
	std::vector<ShaderPtr> shader_list;
	
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
	double mspf_samples[10];
	int currentSample;

	std::string resource_dir;


};


ProtoGraphicsImplementation *ProtoGraphicsImplementation::instance = 0x0;

ProtoGraphicsPtr ProtoGraphics::create() { return std::shared_ptr<ProtoGraphics>(new ProtoGraphicsImplementation); }
