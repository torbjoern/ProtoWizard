#include <protographics.h>
#include "color_utils.h"

#include "../../depends/bass_wrapper/bass_wrapper.h"

#include <iostream>

glm::vec2 pointOnCicle( float u )
{
	return glm::vec2( cos(u), sin(u) );
}

const float PI = 3.141592f;
const float TWO_PI = 2.f * 3.141592f;
const glm::mat4 identityMatrix(1.f);

struct SpectrumViz
{
	SpectrumViz()
	{
		for(unsigned i=0; i<num_layers; i++)
		{
			heights[i].resize( num_bands );
		}
	}

	void setFrontBand( int pos, float value )
	{
		heights[0][pos] = value;
	}

	void PushBack( )
	{
		for( size_t layer = num_layers-1; layer > 0; layer-- )
		{
			heights[layer] = heights[layer-1];
		}

	}

	void Draw(ProtoGraphics &protoGraphics, int color_scheme_1)
	{	
		float SPECWIDTH = 100;

		protoGraphics.setColor( 0.5f, 0.0f, 0.f );
		protoGraphics.setScale( 0.5f * num_bands, 0.5f, 0.5f * num_layers );
		protoGraphics.drawCube( glm::vec3( 0.f, -1.f, 0.f ) ); // num_bands 96
		protoGraphics.setScale( 1.f );

		

		float barStep = SPECWIDTH/float(num_bands-1) * .75f;
		float radi = 0.5f * barStep;

		for( size_t layer = 0; layer < num_layers; layer++ )
		{

			std::vector< float > &active_layer = heights[layer];

			for( size_t i=0; i<active_layer.size(); i++ )
			{
				float x = (float)i;
				float y = active_layer[i];
				float z = 25.f + layer * -radi*2.f;

				if ( y < 0.1f ) continue;

				float xp = -0.5f * SPECWIDTH + barStep/2.0f + x * (SPECWIDTH / num_bands);

				//protoGraphics.setAlpha( start_alpha - (layer / (float)num_layers) );

				float r,g,b;
				switch ( color_scheme_1 )
				{
				case 0:
					protowizard::hsv_to_rgb( 360.0f - (y / 25.f * 360.f), 1.0f, 0.5f + y/12.f, r, g, b );
					protoGraphics.setColor( r, g, b);
					break;
				case 1:

					protowizard::hsv_to_rgb( 360.0f - (y / 20.f * 360.f), 1.f, 1.f, r, g, b );
					protoGraphics.setColor( r, g, b);
					break;
				default:
					protoGraphics.setColor( y/20.f , 1.f - y/20.f, 0.f);
				}

				// the old
				//protoGraphics.drawCone( glm::vec3(xp,0.1f,z), glm::vec3(xp,0.1f + y,z), radi );

				// the new
				protoGraphics.setScale( radi, y * 0.5f, radi );
				protoGraphics.drawCube( glm::vec3( xp, y * 0.5f, z ) );
			}
		}
		protoGraphics.setScale( 1.f );

	}

	// 96 layers and 128 bands looks good. Captures a lot of features
	static const int num_layers = 128;  // 64, z-layers going back in time!
	static const int num_bands = 256;   // 96, a spectrum band
	std::vector< float > heights[num_layers];
};

void UpdateLogSpectrum3D(BassWrapper &bassWrapper , SpectrumViz &spectrumViz )
{
	float SPECHEIGHT = 25.f;

	int b0 = 0;
	int bands = spectrumViz.num_bands;
	for (int x=0; x<bands; x++)
	{
		float sum = 0.0f;
		float sc = 0;
		int b1 = (int) pow(2.0f, x*10.0f / (bands-1.f) );
		if( b1 > 1024 ) b1 = 1023;
		if( b1 <= b0 ) b1 = b0+1; // use at least 1 FFT bin
		sc = 10.0f + b1 - b0;

		while(b0 < b1)
		{
			sum += bassWrapper.get_fft_value(1 + b0);
			b0 += 1;
		}

		float y = sqrtf( sum / log10f(sc) ) * 1.7f * SPECHEIGHT-4; // scale it
		if( y > SPECHEIGHT ) y = SPECHEIGHT; // cap it
		if( y < 0.0f ) y = 0.0f; // clamp it


		spectrumViz.setFrontBand( x, y );

	}
}

void tunnel( ProtoGraphics& proto, BassWrapper &bassWrapper )
{
	int num_blocks_long = 32;
	int num_circle = 32;

	proto.setColor(1.f, 0.f, 0.f);
	float block_size = 1.f;

	float time = proto.klock();

	proto.setScale(0.75f);

	for(int i=0; i<num_blocks_long; ++i){
		
		float z_pos = (float)i; // / float(num_blocks_long);
		float normalized_i = i / float(num_blocks_long);
		
		proto.setColor( protowizard::hsv2rgb( fmod( (time + normalized_i) * 180.f, 360.f), 1,1 ) ); 

		float beat_scale = 5.f * sin(time + PI * normalized_i);
		float scale = 10.f + beat_scale;

		//float sk = 0.75f + std::max(0.f, 0.5f*beat_scale);
		//proto.setScale( sk, sk, 0.75f );

		for(int j=0; j<num_circle; ++j){
			glm::vec2 pt = pointOnCicle( TWO_PI * j / float(num_circle) );
			glm::vec2 scaled = scale * pt;
			glm::vec3 pos( scaled.x, scaled.y, z_pos );
			//proto.setOrientation( glm::vec3(pt.x, pt.y, 0.f), 0.f );
			proto.setOrientation( glm::rotate(identityMatrix, 90.f, glm::vec3(-pt.y, pt.x, 0.f) ) );

			proto.drawCube( pos );
		}
	}

	proto.setScale( 1.f );
}

void circle_of_birds( ProtoGraphics& proto )
{
	int num_birds = 18;
	for ( int i=0; i<num_birds; i++ ){
		float param = i / float(num_birds);
		float ang =  param * (2.f * PI);
		glm::vec2 point = 5.f * pointOnCicle( ang ); 
		float normalized = ang/(2.f * PI);
		float hang = normalized * 360.f;
		proto.setColor( hang, 1,1 ); 
		proto.setTexture("assets/textures/whitehen.tga");
		proto.drawMesh( glm::vec3( point.x, 0.f, point.y), -90.f + hang, 0.f, "assets/models/googley_chicken.obj");
	}
}

void DrawLogSpectrumFast(ProtoGraphics &protoGraphics, BassWrapper &bassWrapper)
{
	int xres = protoGraphics.getWindowWidth();
	int yres = protoGraphics.getWindowHeight();
	int SPECWIDTH = xres;
	float SPECHEIGHT = 200.f;

	int b0 = 0;
	int bands = 128;
	for (int x=0; x<bands; x++)
	{
		float sum = 0.0f;
		float sc = 0;
		int b1 = (int) pow(2.0f, x*10.0f / (bands-1.f) );
		if( b1 > 1024 ) b1 = 1023;
		if( b1 <= b0 ) b1 = b0+1; // use at least 1 FFT bin
		sc = 10.f + b1 - b0;

		// b0 = 0 this destroys the bars, makes them all the same height
		while(b0 < b1)
		{
			//sum += fft[1 + b0];
			//sum += bassWrapper.get_prev_fft_value(1 + b0);
			sum += bassWrapper.get_fft_value(1 + b0);
			b0 += 1;
		}

		float y = sqrtf( sum / log10f(sc) ) * 1.7f * SPECHEIGHT-4; // scale it
		if( y > SPECHEIGHT ) y = SPECHEIGHT; // cap it
		y -= 1.0f;

		float barStep = SPECWIDTH/float(bands) * .75f;

		protoGraphics.setColor(1.f, 1.f, 0.f);
		protoGraphics.moveTo(  barStep/2.0f + x * (SPECWIDTH / (float)bands), (float)(yres) );
		protoGraphics.lineTo(  barStep/2.0f + x * (SPECWIDTH / (float)bands), (float)(yres-y) );
	}
}

int main()
{
	ProtoGraphics proto;

	if (!proto.init(640,480) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}

	BassWrapper bass_wrap;
	bass_wrap.list_recording_devices();
	int recording_device = -1;
	std::cout << "please type a recording device handle: ";
	std::cin >> recording_device;
	
	bass_wrap.init_loopback( recording_device );

	SpectrumViz spectrumViz;

	proto.setCamera( glm::vec3(0.f, 0.f, -15.f), 0.f, 0.f );
	size_t numframes = 0;

	glm::vec3 cam_pos(0.f, 2.f, -15.f);
	glm::vec3 cam_target = cam_pos + glm::vec3(0.f, 0.f, 1.f);

	glm::vec3 chicken_pos( 0.f );
	glm::vec3 chicken_dir( 0.f );
	float chicken_horiz_dir = 0.f;
	float chicken_verti_dir = 0.f;


	float normalized_mousx = proto.getMouseX() / (float)proto.getWindowWidth();
	float normalized_mousy = proto.getMouseY() / (float)proto.getWindowHeight();
	glm::vec2 old_mouse_pos( normalized_mousx, normalized_mousy );

	float oldtime = 0.f;

	float secTimer = 0.015f;

	proto.setTitle( "your friendly example project" );
	proto.setFrameRate( 60 );
	do
	{
		bass_wrap.update();

		secTimer-=proto.getMSPF();

		if ( secTimer <= 0.0 )
		{
			secTimer = 0.015f;

			spectrumViz.PushBack();
		}

		proto.cls(0.15f, 0.15f, 0.15f);

		proto.setColor(0.f, 1.f, 0.f);
		proto.disableTexture();

		//tunnel( proto, bass_wrap );


		DrawLogSpectrumFast( proto, bass_wrap );

		UpdateLogSpectrum3D( bass_wrap, spectrumViz );
		spectrumViz.Draw( proto, 2 );

		float normalized_mousx = proto.getMouseX() / (float)proto.getWindowWidth();
		float normalized_mousy = proto.getMouseY() / (float)proto.getWindowHeight();
		glm::vec2 normalized_mouse_pos( normalized_mousx, normalized_mousy );

		proto.setColor( 0.8f, 0.8f, 0.8f );

		if ( proto.mouseDownLeft() )
		{
			glm::vec2 mouse_speed = normalized_mouse_pos - old_mouse_pos;
			chicken_horiz_dir += 90.f * mouse_speed.x;
			chicken_verti_dir -= 90.f * mouse_speed.y;
		}
		old_mouse_pos = normalized_mouse_pos;

		glm::mat4 identityMatrix(1.f);
		glm::mat4 xrot = glm::rotate( identityMatrix, chicken_verti_dir, glm::vec3(1.f, 0.f, 0.f) );
		glm::mat4 yrot = glm::rotate( identityMatrix, chicken_horiz_dir, glm::vec3(0.f, 1.f, 0.f) );
		glm::mat4 final = xrot * yrot;

		float control_dx = float( proto.keystatus('A') - proto.keystatus('D') );
		float control_dz = float( proto.keystatus('W') - proto.keystatus('S') );

		chicken_dir = ( glm::vec4(0.f, 0.f, 1.f, 0.f) * final ).xyz();


		if ( fabs(control_dx) > 0.0f || fabs(control_dz) > 0.0f )
		{
			float units_per_sec = 30.f;
			float speed = units_per_sec * proto.getMSPF(); 

			chicken_pos += control_dz * speed * chicken_dir;
			const glm::vec3 up_dir = glm::vec3(0.f, 1.f, 0.f);
			chicken_pos += control_dx * speed * glm::cross( up_dir, chicken_dir ); // ambigous
		}

		
		//proto.setOrientation( glm::rotate( glm::transpose(final), 90.f, glm::vec3(0.f, 1.f, 0.f) ) );
		//proto.setColor(1.f, 1.f, 1.f); 
		//proto.setTexture("assets/textures/whitehen.tga");
		//proto.drawMesh( chicken_pos, "assets/models/googley_chicken.obj");
		//proto.setOrientation( identityMatrix );
		
		

		//proto.setCamera( cam_pos, cam_target, glm::vec3(0.f, 1.f, 0.f) );
		proto.setCamera( chicken_pos, chicken_pos+chicken_dir, glm::vec3(0.f, 1.f, 0.f) );
		

		proto.frame();
		numframes++;
	} while( proto.isWindowOpen() );


}