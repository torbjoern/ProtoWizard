#pragma comment(lib, "glfw_d.lib")
#pragma comment(lib, "opengl32.lib")

#include <glm.hpp>

#include <gl/glfw.h>
#include <cmath>



class ProtoGraphics
{
public:
	ProtoGraphics()
	{

	}
	~ProtoGraphics()
	{

	}

	bool init()
	{
		int ok = glfwInit();
		if ( ok==0 ) return false;
		xres = 800;
		yres = 600;

		// TODO: use GL 3.3 and core profile that wont run any deprecated stuff
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
		//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		ok = glfwOpenWindow(xres,yres,8,8,8,8,16,16,GLFW_WINDOW);
		if (ok == 0 ) return false;

		glfwSetWindowTitle("ProtoWizard script server");

		glfwSwapInterval(1);

		// Setup opengl matrix state so we can draw 2D stuff
		initGL();

		// Create circle vertices at origin
		initCircle();
		

		return true;
	}

	void shutdown()
	{
		glfwCloseWindow();
		glfwTerminate();
	}

	void frame()
	{
		glfwGetMousePos(&mousx, &mousy);

		glfwSwapBuffers();
	}

	int getMouseX() 
	{
		return mousx;
	}

	int getMouseY() 
	{
		return mousy;
	}

	void drawCircle( float x, float y, float radius )
	{
		glLoadIdentity();
	
		glTranslatef( x, y, 0.0f);
		// Get the scaling factor
		float scale_factor = radius / circleInitRadius; 
		glScalef( scale_factor,scale_factor,scale_factor );

		// vi gjøre maskinen forberet på typen data som skal lastes opp
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glTexCoordPointer( 2, GL_FLOAT, sizeof(TexturedVertex), &circle_vertices[0].u ); 
		glVertexPointer( 2, GL_FLOAT, sizeof(TexturedVertex), &circle_vertices[0].x );

		glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_SEGMENTS+1);


		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void cls( float r, float g, float b )
	{
		glClearColor( r,  g,  b, 1.0f );
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
	}

	bool isWindowOpen()
	{
		return glfwGetWindowParam( GLFW_OPENED ) == 1;
	}
private:


	// TODO perhaps move opengl-specific stuff to its own file so its easier to create a Direct3D painter or just manage GL-state.
	//GLuint make_buffer( GLenum target, const void *buffer_data, GLsizei buffer_size)
	//{
	//		GLuint buffer;
	//		glGenBuffers(1, &buffer);
	//		glBindBuffer(target, buffer);
	//		glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
	//		return buffer;
	//}

	void initGL()
	{
		// TODO remove these and use VSML
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( 0,xres,yres,0,-1,1 );  // Konstruer en ortografisk matrise for 2D bruk

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		glfwGetMousePos(&mousx, &mousy);
	}

	void initCircle()
	{
		circleInitRadius = 100.0f;
		circle_vertices[0].set( 0.0f, 0.0f, 0.0f, 0.0f );

		for (int i=0; i<CIRCLE_SEGMENTS; i++)
		{
			float a = i * 6.28f / (CIRCLE_SEGMENTS-1);
			float ca = cos(a)*circleInitRadius;
			float sa = sin(a)*circleInitRadius;
			circle_vertices[i+1].set( ca, sa, 0.0f, 0.0f );
		}
	}


private:
	struct TexturedVertex
	{
		float x,y;
		float u,v;
		void set(float xx,float yy,float uu, float vv){x = xx; y = yy; u = uu; v = vv;}
	};

	int xres, yres;
	int mousx, mousy;

	static const int CIRCLE_SEGMENTS = 36;
	TexturedVertex circle_vertices[CIRCLE_SEGMENTS+1];
	float circleInitRadius;

};