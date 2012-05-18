#include <proto/protographics.h>
#include <proto/color_utils.h>
#include <string>

void draw_models( protowizard::ProtoGraphicsPtr proto )
{
	proto->setScale( 1.f );
	static float horiz = 0.f;
	horiz += proto->getMSPF() * 45.f;

	proto->disableTexture();
	
	proto->setTexture( proto->getResourceDir() + "/textures/googley_hen.jpg");
	proto->drawMesh( glm::vec3(0.f, 0.5f, 0.f), horiz, 90.f, proto->getResourceDir() + "/models/googley_chicken.obj");
	//proto->drawMesh( glm::vec3(0.f, 0.5f, 0.f), horiz, 90.f, proto->getResourceDir() + "/models/non_existant.obj");


	proto->setTexture( proto->getResourceDir() + "/textures/cube.png");
	proto->drawMesh( glm::vec3(0.f, 0.f, 0.f), horiz, 0, proto->getResourceDir() + "/models/cube.obj");
}


int main(int argc, const char* argv[])
{
	protowizard::ProtoGraphicsPtr proto = protowizard::ProtoGraphics::create();

	if (!proto->init(640,480) ) {
		throw "failed to init proto-> remembered to set default data dir???";
		return 1;
	}


	proto->setFrameRate( 60 );
	
	char title_buf[256];
	do
	{
		proto->cls(0.f, 0.f, 0.f);

		float mspf = proto->getMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, %3.0f", 1000.f * mspf, 1.f/mspf);
		proto->setTitle( title_buf );

		float normalized_mx = proto->getMouseX() / (float)proto->getWindowWidth();
		float normalized_my = proto->getMouseY() / (float)proto->getWindowHeight();
		glm::vec2 normalized_mouse(normalized_mx, normalized_my);

		float zoom = normalized_my * 5.f;
		proto->getCamera()->lookAt( glm::vec3(0.f, zoom, -zoom), glm::vec3(0.f, 1.0f, 0.f), glm::vec3(0.f, 1.f, 0.f) );
		draw_models( proto );

		//float ang = normalized_mouse.x*6.28f;
		//glm::vec3 cam_pos( cos(ang), 0.f, sin(ang) );
		//cam_pos *= 25.f;
		//proto->setCamera( cam_pos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f) );

		proto->frame();

	} while( proto->isWindowOpen() );


}