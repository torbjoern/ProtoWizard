
#include "protographics.h"

#include <vector>
#include <algorithm>
#include <functional>



#include "common.h"

#include "depends/noise/perlin.h"

//using namespace Proto;

ProtoGraphics* ProtoGraphics::instance = 0x0; // C++ hack, static variables must be instantiated in a cpp


ProtoGraphics::ProtoGraphics()
{
	instance = this;

	isRunning = false;

	time = 0.0;
	old_time = 0.0;
	delta_time = 0.0;

	max_millis_per_frame = 1.0 / 1000.0; // cap framerate at 1000 fps

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
	scale = glm::vec3( 1.0f );
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

bool ProtoGraphics::init(int xres, int yres)
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
	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	ok = glfwOpenWindow(xres,yres,8,8,8,8,24,8,GLFW_WINDOW); // Important to either use 32 bit depth, or 24d8s
	if (ok == 0 ) return false;


	glfwSetWindowTitle("ProtoWizard script server");
	glfwSwapInterval(0); // vsync on/off
		
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
	
	if ( geo_shader_normals.install("assets/passthru.vert", "assets/normals.gs", "assets/normals.frag") == false )
	{
		return false;
	}else{
		shader_list.push_back( &geo_shader_normals );
	}
		

	//glEnable(GL_MULTISAMPLE);
	GetError();
			
	isRunning = true;
	return true;
}


void ProtoGraphics::setCamera( glm::vec3 pos, glm::vec3 target, glm::vec3 up )
{
	camera.lookAt( pos, target, up );
}

void ProtoGraphics::setCamera( float x, float y, float z, float hang, float vang )
{
	camera.set( glm::vec3(x,y,z), hang, vang );
}


void ProtoGraphics::setCamera( glm::vec3 pos, float hang, float vang )
{
	camera.set( pos, hang, vang );
}

void ProtoGraphics::shutdown()
{	
	geo_lib.shutdown();

	for(unsigned int i=0; i<shader_list.size(); i++){
		shader_list[i]->shutdown();
	}

	isRunning = false;

	glfwCloseWindow();
	glfwTerminate();
}

void ProtoGraphics::cls( float r, float g, float b )
{
	assert( isRunning );
	glClearColor( r,  g,  b, 1.0f );
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
}

void ProtoGraphics::frame()
{	
	assert( isRunning );
	GetError();
	
	//camera.update( keystatus(GLFW_KEY_LEFT), keystatus(GLFW_KEY_RIGHT), keystatus(GLFW_KEY_UP), keystatus(GLFW_KEY_DOWN), (float)getMouseX(), (float)getMouseY(), mouseDownLeft(), delta );
	//camera.update( keystatus('A'), keystatus('D'), keystatus('W'), keystatus('S'), (float)getMouseX(), (float)getMouseY(), mouseDownLeft(), delta );

	light_pos = glm::vec3(5.f, 5.f, 5.f );
	//setEmissive( glm::vec3( 1.f ) );
	//drawSphere( light_pos, 1.0f );
	//setEmissive( glm::vec3( 0.f ) );


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

	if ( keyhit('2') )
	{
		//setTexture( depth_texture );
		//glQuad();
	}


	glfwGetMousePos(&mousx, &mousy);

	double time_since_program_started = glfwGetTime();
	delta_time = time_since_program_started - old_time;
	old_time = time_since_program_started;

	time += delta_time;

	double time_to_sleep = 0.0;
	if ( delta_time < max_millis_per_frame ) {
		time_to_sleep = max_millis_per_frame - delta_time;
		if ( time_to_sleep > 0.0 ) {
			glfwSleep( time_to_sleep );
		}
	}

	char title_buf[256];
	sprintf_s(title_buf, 256, " %2.2f mspf, %2.2f mspf with sleep,  numObjs = %i, max = %i", delta_time*1000.0, (delta_time+time_to_sleep)*1000.0, num_objs, max_objs);
	glfwSetWindowTitle(title_buf);

	numframes++;
	glfwSwapBuffers();
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

float ProtoGraphics::klock()
{
	return (float) time;
}

int ProtoGraphics::getMouseX() 
{
	return mousx;
}

int ProtoGraphics::getMouseY() 
{
	return mousy;
}

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

FirstPersonCamera& ProtoGraphics::getCamera()
{
	return camera;
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

void ProtoGraphics::setEmissive( glm::vec3 emissive )
{
	emissiveColor = emissive;
}

void ProtoGraphics::setOrientation( const glm::mat4 &ori ){
	currentOrientation = ori;
}

void ProtoGraphics::setScale( float x, float y, float z )
{
	scale = glm::vec3(x,y,z);
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
		blend_state = blending::ADDITIVE_BLEND;
	}else{
		blend_state = blending::SOLID_BLEND;
	}
		
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

void ProtoGraphics::drawCircle( float x, float y, float radius )
{
	CircleState state;
	state.color = colorState;
	state.x = x;
	state.y = y;
	state.radius = radius;
	buffered_circles.push_back( state );
}

void ProtoGraphics::drawSphere( glm::vec3 position, float radius ) 
{
	SphereState *state = new SphereState;
	state->color = colorState;
	state->emissiveColor = emissiveColor;
	state->transform = glm::translate( glm::mat4( 1.0f ), position );
	state->radius = radius;
	state->blend_mode = blend_state;
	buffered_shapes.push_back( state );

	sphereList.push_back( state );
}

void ProtoGraphics::drawCone( glm::vec3 p1, glm::vec3 p2, float radius ) 
{
	CylinderState *state = new CylinderState;
	state->color = colorState;
	state->emissiveColor = emissiveColor;
	state->p1 = p1;
	state->p2 = p2;
	state->radius = radius;
	state->blend_mode = blend_state;
	buffered_shapes.push_back( state );

	cylinderList.push_back( state );
}

void ProtoGraphics::drawCube( glm::vec3 position, float radius )
{
	CubeState *state = new CubeState;
	state->color = colorState;
	state->emissiveColor = emissiveColor;
	state->transform = glm::translate( glm::mat4(1.0), position );
	state->transform *= currentOrientation;
	state->transform = glm::scale( state->transform, scale );
		
		
	state->radius = radius;
	state->blend_mode = blend_state;
	buffered_shapes.push_back( state );

	cubeList.push_back( state );
}

void ProtoGraphics::drawPlane( glm::vec3 position, glm::vec3 normal, float radius )
{
	PlaneState *state = new PlaneState;
	state->color = colorState;
	state->emissiveColor = emissiveColor;
	state->transform = glm::translate( glm::mat4( 1.0f ), position );
	state->normal = normal;
	state->radius = radius;
	state->blend_mode = blend_state;
	buffered_shapes.push_back( state );

	planeList.push_back( state );
}

void ProtoGraphics::drawRoundedCube(glm::vec3 pos, float radius, float edge_radius)
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

	v0 += pos;
	v1 += pos;
	v2 += pos;
	v3 += pos;
	v4 += pos;
	v5 += pos;
	v6 += pos;
	v7 += pos;

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

	glm::vec3 n0 ( 0.f, 0.f, -1.f );
	glm::vec3 n1 ( 0.f, 0.f, +1.f );
	glm::vec3 n2 ( 0.f, -1.f, 0.f );
	glm::vec3 n3 ( 0.f, +1.f, 0.f );
	glm::vec3 n4 ( -1.f, 0.f, 0.f );
	glm::vec3 n5 ( +1.f, 0.f, 0.f );
		
	float radius2 = (radius + edge_radius);
	drawPlane(pos + n0*radius2, n0, radius );
	drawPlane(pos + n1*radius2, n1, radius );
	drawPlane(pos + n2*radius2, n2, radius );
	drawPlane(pos + n3*radius2, n3, radius );
	drawPlane(pos + n4*radius2, n4, radius );
	drawPlane(pos + n5*radius2, n5, radius );

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
	shader_lines2d.begin();
	unsigned int loc = shader_lines2d.GetVariable("mvp");
	glm::mat4 orthomat = glm::ortho(  0.f, (float)xres, (float)yres, 0.f );
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(orthomat) );
	geo_lib.line.draw(buffered_lines);
	buffered_lines.clear();
}

void ProtoGraphics::draw_buffered_circles()
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
	// send already init-ed ortho to shader
	vsml->matrixToUniform(VSML::PROJECTION); 

	for (unsigned int i=0; i<buffered_circles.size(); i++)
	{
		float x = buffered_circles[i].x;
		float y = buffered_circles[i].y;
		float radius = fabs(buffered_circles[i].radius);

		glm::vec4 color = buffered_circles[i].color;
		shader_2d.SetVec4( colorLoc, color );

		vsml->loadIdentity(VSML::MODELVIEW);
		vsml->translate(x, y, 0.0f);
		// Get the scaling factor
		float scale_factor = radius; 
		vsml->scale(scale_factor,scale_factor,scale_factor);

		vsml->matrixToUniform(VSML::MODELVIEW);

		if ( buffered_circles[i].radius < 0.f )
		{
			geo_lib.circle.draw_open();
		} 
		else
		{
			geo_lib.circle.draw_fill();
		}
		
	}
	buffered_circles.clear();
}

void ProtoGraphics::init_phong( Shader& active_shader )
{
	active_shader.SetVec3( active_shader.GetVariable("cameraSpaceLightPos"), light_pos );
	active_shader.SetFloat( active_shader.GetVariable("shininess"), 10.0f );
}

void ProtoGraphics::draw_buffered_shapes()
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
	

	Shader& active_shader_ref = phong_shader; //  phong_shader geo_shader_normals
	active_shader_ref.begin();
	init_phong( active_shader_ref );

	unsigned int worldLoc = active_shader_ref.GetVariable("worldMatrix");
	unsigned int viewLoc = active_shader_ref.GetVariable("viewMatrix");
	unsigned int projLoc = active_shader_ref.GetVariable("projMatrix");
		
	glm::mat4 projection =
		glm::perspective(75.0f, xres/(float)yres, 1.0f, 1000.f);
	glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr(projection) );

	glm::mat4 viewMatrix = camera.getViewMatrix();
	glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix) );


	unsigned int colorLoc = active_shader_ref.GetVariable("diffuseColor");

	//////////////////////////////////////////////////////////////////////////


	// useful article on blending
	// http://blogs.msdn.com/b/shawnhar/archive/2009/02/18/depth-sorting-alpha-blended-objects.aspx

	for (unsigned int i=0; i<opaque.size(); i++)
	{
			BaseState3D *state = opaque[i];
			active_shader_ref.SetVec4( colorLoc, state->color );
			state->pre_draw( active_shader_ref );
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
		//std::sort( translucent.begin(), translucent.end(), SortFunctor(camera) );

		// http://www.opengl.org/sdk/docs/man/xhtml/glDepthMask.xml
		glDepthMask(GL_FALSE);

		//http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Don't care about alpha of what we write over
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blending incoming and inversely what was there before
		glEnable(GL_BLEND);


		int num_objs = translucent.size();
		//for (int i=0; i<num_objs; i++)
		//for (unsigned int i=num_objs-1; i>0; i--)
		for (int i=0; i<num_objs; i++)
		{
			BaseState3D *state = translucent[i];
			active_shader_ref.SetVec4( colorLoc, state->color );
			state->pre_draw( active_shader_ref );
			translucent[i]->draw( &geo_lib );
		}

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}



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

	sphereList.clear();
	cylinderList.clear();
	planeList.clear();
	cubeList.clear();

	for(unsigned i=0; i<buffered_shapes.size(); i++)
	{
		//delete buffered_shapes[i];
	}

	buffered_shapes.clear();

	GetError( __FUNCSIG__ );
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