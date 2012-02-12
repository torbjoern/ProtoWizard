#include <protographics.h>
#include "color_utils.h"
#include "math/math_common.h"

#include "../../depends/bass_wrapper/bass_wrapper.h"

#include <iostream>

glm::vec2 pointOnCicle( float u )
{
	return glm::vec2( cos(u), sin(u) );
}

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
		// black floor
		protoGraphics.setColor( 0.f, 0.f, 0.f );
		protoGraphics.setScale( num_bands, 0.5f, num_layers ); // scale it
		//protoGraphics.drawCube( glm::vec3( 0.f, -1.f, -.5f * num_layers ) ); // center it
		protoGraphics.drawCube( glm::vec3( 0.f, -1.f, 0.f ) ); // center it
		protoGraphics.setScale( 1.f );

		float SPECWIDTH = 2.f * num_bands;
		float spec_height = (float)spectrum_height;
		

		float barStep = SPECWIDTH/float(num_bands);
		float radi = 0.5f * barStep;

		for( size_t layer = 0; layer < num_layers; layer++ )
		{

			std::vector< float > &active_layer = heights[layer];

			for( size_t i=0; i<active_layer.size(); i++ )
			{
				float x = (float)i;
				float y = active_layer[i];
				float z = layer * -radi*2.f;

				float xp = -num_bands + (x+1.f) * 2.f * radi;
				float yp = spec_height * y;
				if ( yp < 0.01f ) continue;
				
				//protoGraphics.setAlpha( 1.0f - 0.5f * (layer / (float)num_layers) );

				float r,g,b;
				switch ( color_scheme_1 )
				{
				case 0:
					protowizard::hsv_to_rgb( 360.0f - (y / spec_height * 360.f), 1.0f, 0.5f + y/12.f, r, g, b );
					protoGraphics.setColor( r, g, b);
					break;
				case 1:

					protowizard::hsv_to_rgb( 360.0f - (y / spec_height * 360.f), 1.f, 1.f, r, g, b );
					protoGraphics.setColor( r, g, b);
					break;
				default:
					protoGraphics.setColor( .5f*y , 1.f - .5f*y, 0.f);
					// ROYGBIV
					// RED ORANGE YELLOW GREEN BLUE INDIGO VIOLET
					// [0 -> 45 -> 90 -> 135 -> 180 -> 225 -> 275->360]
					
					using std::min;
					using std::max;
					float clamped = max<float>( min<float>(y, 1.0f), 0.f );
					float hue = 135.0f - 135.f*clamped;
					//float sat =(active_layer[i]/5.f) * 360.f;
					protoGraphics.setColor( protowizard::hsv2rgb( hue, 1.f, 1.f) );
					
				}

				// the old
				//protoGraphics.drawCone( glm::vec3(xp,0.1f,z), glm::vec3(xp,0.1f + y,z), radi );

				// the new
				protoGraphics.setScale( radi, 2.f + yp * 0.5f, radi );
				protoGraphics.drawCube( glm::vec3( xp, 2.f + yp * 0.5f, z ) );
			}
		}
		protoGraphics.setScale( 1.f );

	}

	// 96 layers and 128 bands looks good. Captures a lot of features
	static const int num_layers = 32;  // 64, z-layers going back in time!
	static const int num_bands = 24;   // 96, a spectrum band
	static const int spectrum_height = 30;
	std::vector< float > heights[num_layers];
};


struct WaveViz
{
	WaveViz()
	{
		for(unsigned i=0; i<num_layers; i++)
		{
			heights[i].resize( num_bands );
		}

		wave_buf.reserve( num_bands );
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

	void Update( BassWrapper& bassWrapper )
	{
		// TODO what if chancount > 1 ? then numbands is incorrect
		wave_buf.resize( bassWrapper.getChannelCount()*num_bands ); // allocate buffer for data
		bassWrapper.init_sample_capture( wave_buf );

		heights[0].assign( wave_buf.begin(), wave_buf.end() );

		// Slow way of doing the same thing as assign:
		//for(int i=0; i<num_bands; ++i){
		//	setFrontBand(i, wave_buf[i]);
		//}
	}



	void Draw(ProtoGraphics &proto )
	{	
		float SPECWIDTH = 50;

		float barStep = SPECWIDTH/float(num_bands-1) * .75f;
		float radi = 0.25f * barStep;

		float x_begin = -0.5f * SPECWIDTH;

		for( size_t layer = 0; layer < num_layers; layer++ )
		{

			std::vector< float > &active_layer = heights[layer];

			float z = 25.f + layer * -radi;
			glm::vec3 p1(x_begin,0,z);

			for( size_t i=0; i<active_layer.size(); i++ )
			{

				float x = (float)i;
				float y = 50.0f * active_layer[i];

				float xp = x_begin + barStep/2.0f + x * (SPECWIDTH / num_bands);

				glm::vec3 p2(xp, y, z);

				proto.setColor( protowizard::hsv2rgb( active_layer[i] * 1360.f, 1.f, 1.f ) ); 
				proto.drawCone(p1,p2, -0.5f * radi);
				p1 = p2;
			}
		}

	} // end draw

	// 96 layers and 128 bands looks good. Captures a lot of features
	static const int num_layers = 32;  // 64, z-layers going back in time!
	static const int num_bands = 16;   // 96, a spectrum band
	std::vector<float> heights[num_layers];
	std::vector<float> wave_buf;
};

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
		
		int b1 = (int) pow(2.0f, x*10.0f / (bands-1.f) ); // (2^0.1), (2^0.2). ... (2^1.0)
		if( b1 > 1024 ) b1 = 1023;
		if( b1 <= b0 ) b1 = b0+1; // use at least 1 FFT bin
		float sc = 10.f + b1 - b0;

		while(b0 < b1)
		{
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

// Expects 2048 sample fft with 1024 floats
void UpdateLogSpectrum3D(BassWrapper &bassWrapper , SpectrumViz &spectrumViz )
{
	//float SPECHEIGHT = (float)spectrumViz.spectrum_height;
	float SPECHEIGHT = 1.0f;

	int bin0 = 0;
	int bands = spectrumViz.num_bands;
	for (int x=0; x<bands; x++)
	{
		int bin1 = (int) pow(2.0f, x*10.0f / (bands-1.f) );
		if( bin1 > 1024 ) bin1 = 1023;
		if( bin1 <= bin0 ) bin1 = bin0+1; // use at least 1 FFT bin
		int sc = 10 + bin1 - bin0; // must begin at 10 because of log10 below, because log10(10) = 1

		// Sum bins
		float sum = 0.0f;
		while(bin0 < bin1)
		{
			sum += bassWrapper.get_fft_value(1 + bin0);
			bin0 += 1;
		}

		float y = ( (sqrtf( sum / log10f(sc) ) - 0.02f) * 1.7f * SPECHEIGHT); // scale it. subtracting 0.01 from whole expr to remove noise
		if( y > SPECHEIGHT ) 
		{
				y = SPECHEIGHT; // cap it
		}
		if( y < 0.f ) y = 0.f; // clamp it


		spectrumViz.setFrontBand( x, 0.98 * y );

	}
}

void drawWaveform( ProtoGraphics &proto, BassWrapper &bassWrapper, std::vector<float> &wave_buf )
{
		int xres = proto.getWindowWidth();
		int yres = proto.getWindowHeight();

		int SPECWIDTH = xres * 0.5f;
		int SPECHEIGHT = 500;

		proto.setColor(0.f, 0.f, 1.f);
		proto.moveTo( 0, 0 );

		wave_buf.resize( bassWrapper.getChannelCount()*SPECWIDTH ); // allocate buffer for data
		bassWrapper.init_sample_capture( wave_buf );

		int y = yres/2;
		for (int x=0;x<SPECWIDTH;x++) {
			float y = (1.0-wave_buf[x]) * SPECHEIGHT/2.f; // invert y coord because of screen goes (0,0) to (width,height) in 2D
			proto.lineTo( x/(float)SPECWIDTH * xres, 100.f + y );
		}	
}

void drawSpeakers( ProtoGraphics& proto, BassWrapper &bassWrapper )
{
}
void draw_tunnel( ProtoGraphics& proto, BassWrapper &bassWrapper, const glm::vec3& translate_pos, bool isLeft )
{
	static float tunnel_scale = 0.1f;
	static float color_scroll = 0;

	float left, right;

	bassWrapper.getLevel( &left, &right );
	float myLevel = isLeft ? left : right;
	float myColorBegin = isLeft ? 90.f : 180.f;

	if ( myLevel > tunnel_scale ) {
		tunnel_scale = myLevel;
		color_scroll += myLevel * 1.f/16.f;
	} else {
		tunnel_scale -= 1.5f * proto.getMSPF(); // shrink rate pr sec
		if ( tunnel_scale < 0.01f ) {
			tunnel_scale = 0.01f;
		}
	}

	int num_blocks_long = 32;
	int num_circle = 16;

	proto.setColor(1.f, 0.f, 0.f);
	float block_size = 1.f;

	//float time = (float)proto.klock();

	proto.setScale(0.5f);

	for(int i=0; i<num_blocks_long; ++i){
		
		float z_pos = (float)i; // / float(num_blocks_long);
		float normalized_i = i / float(num_blocks_long);
		
		//proto.setColor( protowizard::hsv2rgb( fmod( (time + normalized_i) * 180.f, 360.f), 1,1 ) ); 
		proto.setColor( protowizard::hsv2rgb( myColorBegin + myColorBegin*sin(color_scroll + normalized_i), 1,1 ) ); 

		//float tunnel_scale = 5.f * sin(time + M_PI * normalized_i);
		float scale = 0.5f + 10.f * tunnel_scale * sin( M_PI * (color_scroll+normalized_i) );

		//float sk = 0.75f + std::max(0.f, 0.5f*beat_scale);
		//proto.setScale( sk, sk, 0.75f );

		for(int j=0; j<num_circle; ++j){
			glm::vec2 pt = pointOnCicle( TWO_PI * j / float(num_circle) );
			glm::vec2 scaled = scale * pt;
			glm::vec3 pos( scaled.x, scaled.y , z_pos );
			
			float degrees_rot = RADIANS_TO_DEGREES<float>(atan2(pt.y, pt.x) );
			proto.setOrientation( glm::rotate(identityMatrix, degrees_rot , glm::vec3(0.f, 0.f, 1.f) ) );

			proto.drawCube( translate_pos + pos );
		}
	}
	proto.setOrientation( identityMatrix );
	proto.setScale( 1.f );
}

void drawTunnel( ProtoGraphics& proto, BassWrapper &bassWrapper )
{
	glm::vec3 left_pos( -25.f, 25.f, 0.f );
	glm::vec3 rite_pos( +25.f, 25.f, 0.f );
	draw_tunnel( proto, bassWrapper, left_pos, true );
	draw_tunnel( proto, bassWrapper, rite_pos, false );
}

void circle_of_birds( ProtoGraphics& proto )
{
	int num_birds = 18;
	for ( int i=0; i<num_birds; i++ ){
		float param = i / float(num_birds);
		float ang =  param * (TWO_PI);
		glm::vec2 point = 5.f * pointOnCicle( ang ); 
		float normalized = ang/(TWO_PI);
		float hang = normalized * 360.f;
		proto.setColor( hang, 1,1 ); 
		proto.setTexture("assets/textures/whitehen.tga");
		proto.drawMesh( glm::vec3( point.x, 0.f, point.y), -90.f + hang, 0.f, "assets/models/googley_chicken.obj");
	}
}

void line3d( ProtoGraphics& proto )
{
	//proto.moveTo( glm::vec3(0.f, 0.f, 0.f) );
	//proto.lineTo( glm::vec3(10.f, 0.f, 0.f) );
}

int main(int argc, char* argv[])
{
	int recording_device_num = -1;
	if ( argc > 1 ){
		recording_device_num = atoi( argv[1] );
	}

	ProtoGraphics proto;

	if (!proto.init(640,480) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}

	SpectrumViz spectrumViz;
	WaveViz waveViz;

	BassWrapper bass_wrap;
	bass_wrap.list_recording_devices();

	if ( recording_device_num == -1 )
	{
		std::cout << "please type a recording device handle: ";
		std::cin >> recording_device_num;
		bass_wrap.init_loopback( recording_device_num );
	}

	bass_wrap.init_loopback( recording_device_num );
	bass_wrap.init_fft_storage( spectrumViz.num_bands );

	size_t numframes = 0;
	double last_sec = proto.klock() + 1.0;
	unsigned int last_sec_num_frames = 0;
	int fps = 0.0;

	//glm::vec3 cam_pos(0.f, 2.f, -20.f);
	//glm::vec3 cam_target = cam_pos + glm::vec3(0.f, 0.f, 1.f);

	glm::vec3 chicken_pos = glm::vec3(0.f, 15.f, 25.f);
	glm::vec3 chicken_dir( 0.f );
	float chicken_horiz_dir = 180.f; // DEGS OG RADS FFUUU
	float chicken_verti_dir = 0.f;
	// TODO REMOVE CRAP CODE
		glm::mat4 identityMatrix(1.f);
		glm::mat4 xrot = glm::rotate( identityMatrix, chicken_verti_dir, glm::vec3(1.f, 0.f, 0.f) );
		glm::mat4 yrot = glm::rotate( identityMatrix, chicken_horiz_dir, glm::vec3(0.f, 1.f, 0.f) );
		glm::mat4 final = xrot * yrot;

		chicken_dir = ( glm::vec4(0.f, 0.f, 1.f, 0.f) * final ).xyz();

	proto.setCamera( chicken_pos, chicken_dir, glm::vec3(0.f, 1.f, 0.f) );


	float normalized_mousx = proto.getMouseX() / (float)proto.getWindowWidth();
	float normalized_mousy = proto.getMouseY() / (float)proto.getWindowHeight();
	glm::vec2 old_mouse_pos( normalized_mousx, normalized_mousy );

	float oldtime = 0.f;

	int desired_fps = 120;
	float spectrum_update_rate = 1.0f / desired_fps;
	float secTimer = spectrum_update_rate;

	std::vector<float> wave_buf;
	wave_buf.reserve( proto.getWindowWidth() );

	proto.setTitle( "your friendly example project" );
	proto.setFrameRate( desired_fps );

	do
	{
		bass_wrap.update_fft_capture();

		secTimer -= proto.getMSPF();

		if ( secTimer <= 0.0 )
		{
			secTimer = spectrum_update_rate;

			spectrumViz.PushBack();
			//waveViz.PushBack();
		}

		proto.cls(0.15f, 0.15f, 0.15f);

		proto.setColor(0.f, 1.f, 0.f);
		proto.disableTexture();

		drawTunnel( proto, bass_wrap );

		drawWaveform( proto, bass_wrap, wave_buf );
		//DrawLogSpectrumFast( proto, bass_wrap );

		UpdateLogSpectrum3D( bass_wrap, spectrumViz );
		spectrumViz.Draw( proto, 2 );

		//waveViz.Update( bass_wrap );
		//waveViz.Draw( proto );

		drawSpeakers( proto, bass_wrap );

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
			float units_per_sec = 50.f;
			float speed = units_per_sec * proto.getMSPF(); 

			if ( proto.keystatus(' ') )
			{
				speed *= 4.f;
			}

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
		
		double time_now = proto.klock();
		if ( time_now > last_sec )
		{
			fps = (numframes - last_sec_num_frames);
			last_sec_num_frames = numframes;
			last_sec = time_now + 1.0;

			char title_buf[128];
			sprintf_s( title_buf, 128, "%d fps", fps ); 
			proto.setTitle( title_buf );
		}

		proto.frame();
		numframes++;
	} while( proto.isWindowOpen() );


}