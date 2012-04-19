
#include "protographics.h"

#include <vector>
#include <algorithm>
#include <functional>



#include "common.h"
#include "camera.h"
#include "shapes/shapes.h"
#include "../depends/noise/perlin.h"

#include "texture_manager.h"
#include "mesh_manager.h"
#include "path.h"



//using namespace Proto;

ProtoGraphics* ProtoGraphics::instance = 0x0; // C++ hack, static variables must be instantiated in a cpp

ProtoGraphics::ProtoGraphics()
{
	texture_manager = TextureManager::init();
	mesh_manager = MeshManager::init();
	camera = new FirstPersonCamera;

	printf("protographics v 0.1\n");
	instance = this;

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

	resource_dir = "../assets";
}

ProtoGraphics::~ProtoGraphics()
{
	shutdown();
}

// fast float random in interval -1,1
// source by RGBA: http://www.rgba.org/articles/sfrand/sfrand.htm
float ProtoGraphics::sfrand( void )
{
	unsigned int a=(rand()<<16)|rand();  //we use the bottom 23 bits of the int, so one
	//16 bit rand() won't cut it.
	a=(a&0x007fffff) | 0x40000000;  

	return( *((float*)&a) - 3.0f );
}

float ProtoGraphics::random( float range_begin, float range_end ) 
{
	float normalized = (sfrand()*0.5f) + 0.5f;
	return range_begin + normalized * (range_end-range_begin);
}

double ProtoGraphics::noise(double x)
{
	return noise1(x);
}

double ProtoGraphics::noise(double x, double y)
{
	double vec[] = {x,y};
	return noise2(vec);
}

double ProtoGraphics::noise(double x, double y, double z)
{
	double vec[] = {x,y,z};
	return noise3(vec);
}


float ProtoGraphics::getMSPF(){
	return (float)delta_time;
}

void ProtoGraphics::dump_stats()
{
	printf("------------------------\n");
	printf("num opaque: %d\n", num_opaque);
	printf("num blended: %d\n", num_blended);
}

bool ProtoGraphics::install_shaders()
{
	shader_lines2d = new Shader;
	shader_2d = new Shader;
	phong_shader = new Shader;
	geo_shader_normals = new Shader;

	if ( shader_lines2d->install( resource_dir + "/line2d_shader.vert", resource_dir + "/line2d_shader.frag") == false )
	{
		return false;
	}else{
		shader_list.push_back( shader_lines2d );
	}

	if ( shader_2d->install(resource_dir + "/shader2d.vert", resource_dir + "/shader2d.frag") == false )
	{
		return false;
	}
	shader_list.push_back( shader_2d );

	if ( phong_shader->install(resource_dir + "/phong.vert", resource_dir + "/phong.frag") == false )
	{
		return false;
	}else{
		shader_list.push_back( phong_shader );
	}
	
	if ( geo_shader_normals->install(resource_dir + "/passthru.vert", resource_dir + "/normals.gs", resource_dir + "/normals.frag") == false )
	{
		return false;
	}else{
		shader_list.push_back( geo_shader_normals );
	}

	int tex0 = glGetUniformLocation( phong_shader->getProgram() , "tex0");
	phong_shader->begin();
	glUniform1i(tex0, 0); // sampler "tex0" refers to texture unit 0

#ifdef _DEBUG
	 GetError("ProtoGraphics::install_shaders end"); 
#endif

	return true;
}

bool ProtoGraphics::init(int xres, int yres, const char* argv[] )
{
	std::string folderWithBinary = extractExePath( std::string(argv[0]) );
	setResourceDir( folderWithBinary+"/assets" );

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
		
	//glewExperimental = GL_TRUE;
	//int err = glewInit();
	//if ( GLEW_OK != err )
	//{
	//	printf("Failed to init GLEW\n");
	//	return false;
	//}

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


	Shapes::init();

	if ( !install_shaders() ) return false;
		
	glEnable(GL_MULTISAMPLE);			
	isRunning = true;

#ifdef _DEBUG
	 GetError("ProtoGraphics::init"); 
#endif
	return true;
}


void ProtoGraphics::setCamera( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up )
{
	camera->lookAt( pos, target, up );
}

void ProtoGraphics::setCamera( float x, float y, float z, float hang, float vang )
{
	camera->set( glm::vec3(x,y,z), hang, vang );
}

void ProtoGraphics::setCamera( const glm::vec3& pos, float hang, float vang )
{
	camera->set( pos, hang, vang );
}

void ProtoGraphics::setTitle( const std::string &str)
{
	glfwSetWindowTitle( str.c_str() );
}

void ProtoGraphics::setTexture( const std::string& path )
{
	texture_manager->setTexture( path );
}

void ProtoGraphics::disableTexture()
{
	texture_manager->disableTextures();
}

// shutdown is called by the class destructor
// C++ classes call their own dtor first, then dtors of their members
// this means we can't rely on dtors freeing opengl resources, not that it matters really
// the graphics drivers will clean up any un-released objects anyway once the program has shut down
// cleaning up as an exercise.
void ProtoGraphics::shutdown()
{	
	texture_manager->shutdown( texture_manager );
	mesh_manager->shutdown( mesh_manager );
	Shapes::de_init();
	delete camera;

	for(unsigned int i=0; i<shader_list.size(); i++){
		shader_list[i]->shutdown();
	}

	glfwCloseWindow();
	glfwTerminate();

	isRunning = false;
}

void ProtoGraphics::cls( float r, float g, float b )
{
	assert( isRunning );
	glClearColor( r,  g,  b, 1.0f );
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
#ifdef _DEBUG
	GetError("ProtoGraphics::cls");
#endif
}

void ProtoGraphics::frame()
{	
	assert( isRunning );
#ifdef _DEBUG
	GetError("ProtoGraphics::frame begin");
#endif

	if ( false ) //keyhit('R') )
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

	static size_t max_objs = 0;
	size_t num_objs = buffered_shapes.size();
	if ( num_objs > max_objs ){ max_objs = num_objs; }


	draw_buffered_objects();

	glfwGetMousePos(&mousx, &mousy);

	double time_since_program_started = glfwGetTime();
	delta_time = time_since_program_started - old_time;
	old_time = time_since_program_started;

	// time program har spent running. minimized state not counted
	time += delta_time;

	double time_to_sleep = 0.0;
	if ( delta_time < max_millis_per_frame ) {
		time_to_sleep = max_millis_per_frame - delta_time;
		if ( time_to_sleep > 0.0 ) {
			glfwSleep( time_to_sleep );
		}
	}

	//printf("slep %d percent of available frametime\n", (int) (100.0 * delta_time / max_millis_per_frame) );

	//char title_buf[256];
	//sprintf_s(title_buf, 256, " %2.2f mspf, %2.2f mspf with sleep,  numObjs = %i, max = %i", delta_time*1000.0, (delta_time+time_to_sleep)*1000.0, num_objs, max_objs);
	//glfwSetWindowTitle(title_buf);

#ifdef _DEBUG
	GetError("ProtoGraphics::frame end");
#endif

	numframes++;
	glfwSwapBuffers();

}

void ProtoGraphics::debugNormals( bool enable )
{
	isDebugNormalsActive = enable;
}

bool ProtoGraphics::isWindowOpen()
{
	return isRunning;
}

int ProtoGraphics::getWindowWidth() 
{
	return xres;
}

int ProtoGraphics::getWindowHeight() 
{
	return yres;
}

double ProtoGraphics::klock()
{
	return glfwGetTime();
}

int ProtoGraphics::getMouseX() 
{
	return mousx;
}

int ProtoGraphics::getMouseY() 
{
	return mousy;
}

int ProtoGraphics::getMouseWheel() 
{
	return glfwGetMouseWheel();
}

protomath::Ray ProtoGraphics::getMousePickRay()
{
	protomath::Ray ray;
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

float ProtoGraphics::getNormalizedMouseX() 
{
	return mousx / (float)getWindowWidth();
}

float ProtoGraphics::getNormalizedMouseY() 
{
	return mousy / (float)getWindowHeight();
}

float getNormalizedMouseX(); 
float getNormalizedMouseY(); 

bool ProtoGraphics::mouseDownLeft()
{
	return glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) == 1;
}

bool ProtoGraphics::mouseDownRight()
{
	return glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT ) == 1;
}

bool ProtoGraphics::keystatus(int key)
{
	return key_array[key];
}

bool ProtoGraphics::keyhit(int key)
{
	int times_hit = key_hit_array[key];
	key_hit_array[key] = 0;
	return times_hit > 0;
}

double ProtoGraphics::octaves_of_noise(int octaves, double x, double y, double z){
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

void ProtoGraphics::setColor( glm::vec3 c )
{
	colorState.x = c.x;
	colorState.y = c.y;
	colorState.z = c.z;
}

void ProtoGraphics::setColor( float r, float g, float b )
{
	colorState.r = r;
	colorState.b = b;
	colorState.g = g;
}

void ProtoGraphics::setAlpha( float a )
{
	colorState.a = a;
}

void ProtoGraphics::setOrientation( const glm::mat4 &ori ){
	currentOrientation = ori;
}

void ProtoGraphics::setScale( float x, float y, float z )
{
	scale = glm::vec3(x,y,z);
}

void ProtoGraphics::setScale( float uniform_scale )
{
	scale = glm::vec3( uniform_scale );
}

void ProtoGraphics::setBlend( bool active )
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
		blend_state = blending::ALPHA_BLEND;
	}else{
		blend_state = blending::SOLID_BLEND;
	}
		
}

void ProtoGraphics::setLightBlend()
{
	blend_state = blending::ADDITIVE_BLEND;
}

void ProtoGraphics::setFrameRate( int frames_per_second )
{
	if ( frames_per_second >= 1 )
	{
		max_millis_per_frame = 1.0 / double(frames_per_second);
	}
}

void ProtoGraphics::moveTo( float x, float y )
{
	move_to_state = glm::vec2(x,y);
}

void ProtoGraphics::lineTo( float to_x, float to_y )
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

void ProtoGraphics::save_state( BaseState3D* state )
{
	state->color = this->colorState;
	state->blend_mode = this->blend_state;
	state->tex_handle = texture_manager->getActiveTexture();

	if( state->blend_mode == blending::SOLID_BLEND )
	{
		opaque.push_back( state );
	}else{
		translucent.push_back(state);
	}
}

void ProtoGraphics::drawCircle( float x, float y, float radius )
{
	CircleState state;
	state.color = colorState;
	state.blend_mode = blend_state;
	state.x = x;
	state.y = y;
	state.radius = radius;
	buffered_circles.push_back( state );
}

void ProtoGraphics::drawSphere( glm::vec3 position, float radius ) 
{
	SphereState *state = new SphereState;
	save_state( state );

	state->transform = glm::translate( identityMatrix, position );
	state->transform *= currentOrientation;
	state->transform = glm::scale( state->transform, glm::vec3(radius) );

	buffered_shapes.push_back( state );
	sphereList.push_back( state );
}

void ProtoGraphics::drawCone( glm::vec3 p1, glm::vec3 p2, float radius ) 
{
	CylinderState *state = new CylinderState;
	save_state( state );

	state->p1 = p1;
	state->p2 = p2;
	state->radius = radius;
	buffered_shapes.push_back( state );

	cylinderList.push_back( state );
}

void ProtoGraphics::drawCube( glm::vec3 position  )
{
	CubeState *state = new CubeState;
	save_state( state );

	state->transform = glm::translate( identityMatrix, position );
	state->transform *= currentOrientation;
	state->transform = glm::scale( state->transform, scale );
		
	buffered_shapes.push_back( state );

	cubeList.push_back( state );
}

void ProtoGraphics::drawPlane( glm::vec3 position, glm::vec3 normal, float radius )
{
	PlaneState *state = new PlaneState;
	save_state( state );

	state->transform = glm::translate( identityMatrix, position );
	state->transform *= currentOrientation;
	state->transform = glm::scale( state->transform, glm::vec3(radius) );

	state->normal = normal;
	buffered_shapes.push_back( state );

	planeList.push_back( state );
}

void ProtoGraphics::drawMesh( glm::vec3 position, float horiz_ang, float verti_ang, std::string path )
{
	// backup current tfrom matrix state
	glm::mat4 backup = currentOrientation;
	
	// create a rotation matrix given horiz- and verti angles
	//currentOrientation = glm::rotate( glm::rotate( identityMatrix, verti_ang, glm::vec3(1.f, 0.f, 0.f) ), horiz_ang, glm::vec3(0.f, 1.f, 0.f) );
	//currentOrientation = glm::rotate( glm::rotate( identityMatrix, horiz_ang, glm::vec3(0.f, 1.f, 0.f) ), verti_ang, glm::vec3(1.f, 0.f, 0.f) );

	glm::mat4 xrot = glm::rotate( identityMatrix, verti_ang, glm::vec3(1.f, 0.f, 0.f) );
	glm::mat4 yrot = glm::rotate( identityMatrix, horiz_ang, glm::vec3(0.f, 1.f, 0.f) );
	currentOrientation = glm::transpose(xrot * yrot);

	// draw, and restore current tform
	drawMesh( position, path );
	currentOrientation = backup;
}

void ProtoGraphics::drawMesh( glm::vec3 position, std::string path )
{
	MeshState *state = new MeshState;
	save_state( state );
	state->mesh_path = path;
	state->mesh_manager = this->mesh_manager; //std::shared_ptr<MeshManager>(mesh_manager);

	state->transform = glm::translate( identityMatrix, position );
	state->transform *= currentOrientation;
	state->transform = glm::scale( state->transform, scale );

	buffered_shapes.push_back( state );

	meshList.push_back( state );
}

void ProtoGraphics::handle_key(int key, int action)
{		
	key_array[key] = (action == GLFW_PRESS);

	if( (action == GLFW_PRESS) )
	{
		key_hit_array[key]++;
	}
}

void ProtoGraphics::draw_buffered_lines()
{		
	shader_lines2d->begin();
	unsigned int loc = shader_lines2d->GetVariable("mvp");
	glm::mat4 orthomat = glm::ortho(  0.f, (float)xres, (float)yres, 0.f );
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(orthomat) );
	Shapes::line.draw(buffered_lines);
	buffered_lines.clear();
}

void ProtoGraphics::draw_buffered_circles()
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
			Shapes::circle.draw_open();
		} 
		else
		{
			Shapes::circle.draw_fill();
		}
		
	}
	buffered_circles.clear();
}

void ProtoGraphics::init_phong( const Shader& active_shader )
{
	active_shader.SetVec3( active_shader.GetVariable("cameraSpaceLightPos"), light_pos );

	int tex0 = glGetUniformLocation( active_shader.getProgram() , "tex0");
	glUniform1i(tex0, 0); // sampler "tex0" refers to texture unit 0
}

void ProtoGraphics::draw_buffered_shapes( const Shader& active_shader_ref )
{
	unsigned int worldLoc = active_shader_ref.GetVariable("worldMatrix");
	unsigned int viewLoc = active_shader_ref.GetVariable("viewMatrix");
	unsigned int projLoc = active_shader_ref.GetVariable("projMatrix");
	glm::mat4 projection =
		glm::perspective( camera->getFov(), xres/(float)yres, .5f, 1000.f);
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
			BaseState3D *state = opaque[i];
			active_shader_ref.SetVec4( colorLoc, state->color );
			state->pre_draw( active_shader_ref );
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
			BaseState3D *state = translucent[i];
			active_shader_ref.SetVec4( colorLoc, state->color );
			state->pre_draw( active_shader_ref );
			translucent[i]->draw();
		}

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

#ifdef _DEBUG
	 GetError("ProtoGraphics::draw_buffered_shapes 3D end"); 
#endif
}

void ProtoGraphics::draw_buffered_objects()
{
	// Draw 3D first

	// We want depth test enabled for 3D
	glEnable( GL_DEPTH_TEST );

	// turn on culling to make things go a little faster
	glEnable( GL_CULL_FACE );

	// http://www.opengl.org/sdk/docs/man/xhtml/glCullFace.xml
	glCullFace( GL_BACK );

	if ( buffered_shapes.size() > 0 )
	{
		num_opaque = opaque.size();
		num_blended = translucent.size();

		if ( isDebugNormalsActive ) {
			geo_shader_normals->begin();
			draw_buffered_shapes( (*geo_shader_normals) );
		}

		phong_shader->begin();
		init_phong( (*phong_shader) );
		draw_buffered_shapes( (*phong_shader) );
		

		opaque.clear();
		translucent.clear();

		for(unsigned i=0; i<sphereList.size(); i++)
		{
			delete sphereList[i];
		}

		for(unsigned i=0; i<cylinderList.size(); i++)
		{
			delete cylinderList[i];
		}

		for(unsigned i=0; i<planeList.size(); i++)
		{
			delete planeList[i];
		}

		for(unsigned i=0; i<cubeList.size(); i++)
		{
			delete cubeList[i];
		}

		for(unsigned i=0; i<meshList.size(); i++)
		{
			delete meshList[i];
		}

		sphereList.clear();
		cylinderList.clear();
		planeList.clear();
		cubeList.clear();
		meshList.clear();


		for(unsigned i=0; i<buffered_shapes.size(); i++)
		{
			//delete buffered_shapes[i];
		}

		buffered_shapes.clear();

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
	 GetError("ProtoGraphics::draw_buffered_shapes 2D end"); 
#endif
}