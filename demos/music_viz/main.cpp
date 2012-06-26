#include <proto/protographics.h>
#include <proto/color_utils.h>
#include <proto/math/math_common.h>

#include "../../depends/bass_wrapper/bass_wrapper.h"

#include <iostream>

glm::vec2 pointOnCicle( float u )
{
	return 0.5f * glm::vec2( cos(u), sin(u) );
}
glm::vec2 normalOnCicle( float u )
{
	return 0.5f * glm::vec2( sin(u), cos(u) );
}

struct SpectrumViz
{
	SpectrumViz() 
		: numColorSchemes(5)
		, currentColorScheme(0)
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

	void  cycleColorScheme()
	{
		currentColorScheme = (currentColorScheme+1) % numColorSchemes;
		printf("color scheme %d\n", currentColorScheme);
	}

	void Draw(protowizard::ProtoGraphics &proto)
	{
		object2world_matrices_radial.clear();
		object2world_matrices_linear.clear();

		// black floor
		proto.setColor( 0.2f, 0.2f, 0.2f );
		proto.setScale( num_bands, 0.5f, num_layers ); // scale it
		proto.drawCube( glm::vec3( 0.f, -30.f, -.5f * num_layers ) ); // center it
		//proto.drawCube( glm::vec3( 0.f, -20.f, 0.f ) ); // center it
		proto.setScale( 1.f );


		glm::mat4 tf(1.f);

		static float alphaFactor = 0.5f;
		if ( proto.keyhit('I') ) {
			alphaFactor+=0.1;
			if ( alphaFactor > 1.f ) alphaFactor = 1.f;
			printf("alpha = %f\n", alphaFactor );
		}
		if ( proto.keyhit('K') ) {
			alphaFactor-=0.1;
			if ( alphaFactor < 0.f ) alphaFactor = 0.f;
			printf("alpha = %f\n", alphaFactor );
		}

		static bool evil = false; // TODO static is bad when > 1 inst ;)
		if ( proto.keyhit('B') ) {
			evil = !evil;
			if ( evil ) proto.setLightBlend();
			else proto.setBlend(false);
		}
		
		float SPECWIDTH = 2.f * num_bands;
		float spectrum_height = 5.0f;
		float barStep = SPECWIDTH/float(num_bands);
		float cylinderRadius = 10.f; //*layer/float(num_layers);
		float radi = (barStep+1.f) / cylinderRadius;

		for( size_t layer = 0; layer < num_layers; layer++ )
		{


			std::vector<float> &active_layer = heights[layer];

			for( size_t i=0; i<active_layer.size(); i++ )
			{
				float x = (float)i;
				float y = active_layer[i];
				float z = layer * -radi;

				float xNormalized = i/float(num_bands);
				float yNormalized = active_layer[i] / spectrum_height;

				float xp = -radi * num_bands + x;
				float yp = (float)spectrum_height * y;
				if ( yp < 0.01f ) continue;
				
				proto.setAlpha( 1.0f - alphaFactor * (layer / (float)num_layers) ); // 1-1/8

				switch ( currentColorScheme )
				{
				case 0:
					proto.setColor( protowizard::hsv2rgb(  360.0f - (50.f * yp), 1.f, 1.f ) );
					break;
				case 1:
					proto.setColor( protowizard::hsv2rgb(  360.0f - (xNormalized * 360.f), std::max(0.f,yp/3.0f), 1.f ) );
					break;
				case 2:
					proto.setColor( protowizard::hsv2rgb(  360.0f - (xNormalized * 360.f), std::max(0.f,yp/2.0f), std::max(0.f,yp/2.f) ) );
					break;
				case 3:
					proto.setColor( protowizard::hsv2rgb(  360.0f - (xNormalized * 360.f), 1.f, std::max(0.f,yp/3.f) ) );
					break;
				case 4:
					// ROYGBIV
					// RED ORANGE YELLOW GREEN BLUE INDIGO VIOLET
					// [0 -> 45 -> 90 -> 135 -> 180 -> 225 -> 275->360]
					proto.setColor( protowizard::hsv2rgb(  360.0f - (xNormalized * 360.f), 1.f, std::max(0.f,yp/3.5f) ) );
					break;
				}


				glm::vec3 scale (radi, .5f * yp, radi);				
				float rot_angle = xNormalized * 2.f * M_PI;
				//glm::vec2 pang = float(num_bands)/3.f * pointOnCicle(rot_angle);
				//glm::vec2 pang = (float(num_bands)/3.f+scale.y) * pointOnCicle(rot_angle);
				glm::vec2 pang = (cylinderRadius+scale.y) * pointOnCicle(rot_angle);

				glm::vec3 pos( pang.x, pang.y, z );
				
				tf = identityMatrix;
				tf = glm::translate(tf,pos);
				tf = glm::rotate(tf, 90.f + glm::degrees(rot_angle), glm::vec3(0.f, 0.f, 1.f) );
				tf = glm::scale(tf,scale);
				//object2world_matrices_radial.push_back(tf);
				proto.setOrientation( tf );
				proto.drawCube(glm::vec3(0.f));

				// Idea: Draw Quad with 2 points at cyl edge, 2 points at FreqAmp. Stretch!


				float radi2 = 0.5f;
				glm::mat4 v2;
				v2 = glm::translate(v2, glm::vec3(xp*.5f, scale.y *.5f-8.f, -radi2*layer+20.f) );
				v2 = glm::scale(v2, glm::vec3(radi2, 1.5f*yp, radi2) );
				//object2world_matrices_linear.push_back( v2 );

#ifdef SLOW_CODE
				proto.setOrientation(identityMatrix);
				//proto.drawCone( glm::vec3(xp,0.1f,z), glm::vec3(xp,0.1f + y,z), radi );
				float radi2 = 0.5f;
				scale = glm::vec3(radi2, 1.5f*yp, radi2);
				proto.setScale( scale.x, scale.y, scale.z );
				pos = glm::vec3 ( xp*.5f, scale.y *.5f-8.f, -radi2*layer+20.f );
				proto.drawCube(pos);
				proto.setScale(1.f, 1.f, 1.f);
#endif
			}
		}

		//proto.drawCubes( object2world_matrices_radial );
		//proto.drawCubes( object2world_matrices_linear );
		
		proto.setScale( 1.f );
		proto.setOrientation(identityMatrix);

	}

	// 96 layers and 128 bands looks good. Captures a lot of features
	static const int num_layers = 64;  // 32|64, z-layers going back in time!
	static const int num_bands = 96;   // 24|96, a spectrum band
	std::vector<float> heights[num_layers];
	std::vector<glm::mat4> object2world_matrices_radial;
	std::vector<glm::mat4> object2world_matrices_linear;

	int currentColorScheme;
	int numColorSchemes;
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



	void Draw(protowizard::ProtoGraphics &proto )
	{	
		float SPECWIDTH = 50;

		float barStep = SPECWIDTH/float(num_bands-1) * .75f;
		float radi = 0.25f * barStep;

		float x_begin = -0.5f * SPECWIDTH;

		for( size_t layer = 0; layer < num_layers; layer++ )
		{
			std::vector<float> &active_layer = heights[layer];

			float z = 25.f + layer * -radi;
			glm::vec3 p1(x_begin,0,z);

			for( size_t i=0; i<active_layer.size(); i++ )
			{

				float x = (float)i;
				float y = 50.0f * active_layer[i];

				float xp = x_begin + barStep/2.0f + x * (SPECWIDTH / num_bands);

				glm::vec3 p2(xp, y, z);

				proto.setColor( protowizard::hsv2rgb( active_layer[i] * 360.f, 1.f, 1.f ) ); 
				proto.drawCone(p1,p2, -0.5f * radi);
				p1 = p2;
			}
		}

	} // end draw

	// 96 layers and 128 bands looks good. Captures a lot of features
	static const int num_layers = 64;  // 64, z-layers going back in time!
	static const int num_bands = 96;   // 96, a spectrum band
	std::vector<float> heights[num_layers];
	std::vector<float> wave_buf;
};

void DrawLogSpectrumFast(protowizard::ProtoGraphics &proto, BassWrapper &bassWrapper)
{
	int xres = proto.getWindowDimensions().x;
	int yres = proto.getWindowDimensions().y;
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

		proto.setColor(1.f, 1.f, 0.f);
		proto.moveTo(  barStep/2.0f + x * (SPECWIDTH / (float)bands), (float)(yres) );
		proto.lineTo(  barStep/2.0f + x * (SPECWIDTH / (float)bands), (float)(yres-y) );
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

void drawWaveform( protowizard::ProtoGraphics &proto, BassWrapper &bassWrapper, std::vector<float> &wave_buf )
{
	int xres = proto.getWindowDimensions().x;
	int yres = proto.getWindowDimensions().y;

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


void drawSpeakers( protowizard::ProtoGraphics &proto, BassWrapper &bassWrapper, const glm::vec3& translate_pos )
{
	float left, right;
	bassWrapper.getLevel( &left, &right );
}

int main(int argc, char* argv[])
{
	int recording_device_num = -1;
	if ( argc > 1 ){
		recording_device_num = atoi( argv[1] );
	}

	protowizard::ProtoGraphics proto;

	if (!proto.init(640,480,"") ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}

	SpectrumViz spectrumViz;
	//WaveViz waveViz;

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
	float oldtime = 0.f;

	int desired_fps = 60;
	float spectrum_update_rate = 1.0f / desired_fps;
	float secTimer = spectrum_update_rate;

	std::vector<float> wave_buf;
	wave_buf.reserve( proto.getWindowDimensions().x );

	proto.setTitle( "Sound Viz" );
	//proto.setFrameRate( desired_fps );
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

		proto.cls(0.f, 0.f, 0.f);

		proto.setColor(0.f, 1.f, 0.f);
		proto.disableTexture();

		
		//drawWaveform( proto, bass_wrap, wave_buf );
		//DrawLogSpectrumFast( proto, bass_wrap );

		UpdateLogSpectrum3D( bass_wrap, spectrumViz );
		spectrumViz.Draw( proto );
		if ( proto.keyhit('C' ) ) {
			spectrumViz.cycleColorScheme();
		}


		//waveViz.Update( bass_wrap );
		//waveViz.Draw( proto );

		double time_now = proto.klock();
		if ( time_now > last_sec )
		{
			fps = (numframes - last_sec_num_frames);
			last_sec_num_frames = numframes;
			last_sec = time_now + 1.0;

			char title_buf[128];
			sprintf_s( title_buf, 128, "%d fps, desired: %d fps", fps, desired_fps ); 
			proto.setTitle( title_buf );
		}

		proto.getCamera()->update( proto.keystatus('A'), proto.keystatus('D'), proto.keystatus('S'), proto.keystatus('W'), (float)proto.getMouseX(), (float)proto.getMouseY(), proto.mouseDownLeft(), (float)proto.getMSPF() );
		proto.frame();
		numframes++;
	} while( proto.isWindowOpen() );
}