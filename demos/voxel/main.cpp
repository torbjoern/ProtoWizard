#include <iostream>

#include "protoapp.h"

#include "stb_image.c"
#include <fstream>
#include <algorithm>

template <typename T>
T clamp( T var, T lo, T hi )
{
	using std::min;
	using std::max;
	return max<T>( min<T>( var, hi ), lo );
}

typedef unsigned char byte;

class MyApp : public ProtoApp
{
public:
	int non_black_pixels;

	MyApp(const char* argv[])
	{
		ProtoApp::ProtoApp(argv);
	}

	void sum_num_voxels( byte* data, int width, int height, int bits_per_pixel  )
	{
		int stride = width * bits_per_pixel;
		for(int i=0; i<height; i++){
			for(int j=0; j<width; j++){
				const unsigned int idx = stride * i + j * bits_per_pixel;
					
				const float oneOver255 = 1.f/255.f;
				// TODO why not just convert to float when loading?
				float r = data[idx + 0] * oneOver255;
				float g = data[idx + 1] * oneOver255;
				float b = data[idx + 2] * oneOver255;
				float sum = (r+g+b) * 1.f/3.f;

				// skip black pixels
				if ( sum < 0.1f ) {
					continue;
				} else {
					non_black_pixels++;
				}
			}
		}

		
	}

	virtual void run()
	{
		non_black_pixels = 0;

		typedef std::vector<unsigned char> image_t;
		
		int num_slices = 145;
		//std::vector<std::vector<unsigned char>> slices;
		//slices.resize( num_slices );

		// expects files that haves names like: 0000.bmp, 0001.bmp etc.
		int width,height,bits_per_pixel;

		const int BUF_SIZE = 256;
		char filename_buf[BUF_SIZE];
		for( int count = 0; count < num_slices; count++ ){
			sprintf_s(filename_buf, BUF_SIZE, "slices/%04d.bmp", count );
			
			unsigned char *data = stbi_load(filename_buf, &width, &height, &bits_per_pixel, 0);

			if ( data == NULL ){
				const char* reason = stbi_failure_reason();
				std::cerr << "could not load " << filename_buf << " cause: " << reason << std::endl;
			}else{
				//const int bytes = width*height*bits_per_pixel * sizeof(unsigned char);
				//slices[count].assign( data, data+bytes );

				sum_num_voxels( data, width, height, bits_per_pixel );
			}
			stbi_image_free(data);

		}

		printf( "VOXELS: %d \n", non_black_pixels );
		proto.shutdown();
		return;

		const float PI = 3.14159265f;
		do{
			proto.cls(1,1,1);

			float a = 3.f;
			float b = 4.f;

			float t = (float)proto.klock();

			//int segs = 200;
			//for(int i=0; i<segs; i++){
			//	float param = i / (segs-1.f);
			//	float x = a*60.f*cos( 2.f*PI * (a*param+t) );
			//	float y = b*60.f*sin( 2.f*PI * (b*param+t) );
			//	proto.drawCircle( xres/2  + x, yres/2 + y, -5 );
			//}

			float normalized_mx = proto.getMouseX() / xres;
			int slice_idx = (int) clamp<float>( normalized_mx * 31, 0, 31 );

			//std::vector<unsigned char> &sliceRef = slices[slice_idx];
			//int stride = width * bits_per_pixel; // doh
			//for(int i=0; i<height; i++){
			//	for(int j=0; j<width; j++){
			//		const unsigned int idx = stride * i + j * bits_per_pixel;
			//		
			//		const float oneOver255 = 1.f/255.f;
			//		// TODO why not just convert to float when loading?
			//		float r = sliceRef[idx + 0] * oneOver255;
			//		float g = sliceRef[idx + 1] * oneOver255;
			//		float b = sliceRef[idx + 2] * oneOver255;
			//		float sum = (r+g+b) * 1.f/3.f;

			//		// skip black pixels
			//		if ( sum < 0.1f ) {
			//			continue;
			//		} 

			//		proto.setColor( r,g,b );
			//		//proto.drawCircle( j, i, 1.0f );
			//		//proto.drawPixel( 10.f+j, 10.f+i );
			//	}
			//}


			proto.frame();
		}while( proto.isWindowOpen() );

		
	};
};

int main(int argc, const char* argv[])
{
	MyApp app(argv);
	app.run();

	return 0;
}