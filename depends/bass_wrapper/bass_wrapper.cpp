#include "bass_wrapper.h"

#define WIN32_LEAN_AND_MEAN
#include "bass.h"

#include <stdio.h>

#define BASS_CONFIG_REC_LOOPBACK	28
//#define BASS_DEVICE_LOOPBACK		8

BassWrapper::BassWrapper()
{
	HWND win = NULL;

	BASS_SetConfig(BASS_CONFIG_REC_LOOPBACK,1);
	// initialize BASS
	if (!BASS_Init(-1,44100,0,win,NULL)) {
		//printf("Bass: Can't initialize device\n");
		throw("Bass Wrap fail\n");
	}

	channel = NULL;

	// Clear stuff
	for(int i=1; i<NUM_SMOOTHING_BUFS; i++)
	{
		memset( previous_fft[NUM_SMOOTHING_BUFS-i], 0, sizeof(float)*1024 );
	}

	memset( fft, 0, sizeof(float)*1024 );

	// gets system volume
	//float vol = BASS_GetVolume();
	//BASS_SetVolume( vol );
}

BassWrapper::~BassWrapper()
{
	if ( is_recording ) {
		int success = BASS_RecordFree();
		if ( success != 1 ) {
			throw char("could not free recording device");
			int error_code = BASS_ErrorGetCode();
		}
	}
	BASS_Free();
}

int BassWrapper::getChannelCount()
{
	BASS_CHANNELINFO ci;
	BASS_ChannelGetInfo(channel,&ci); // get number of channels
	return ci.chans;
}

void BassWrapper::update_fft_capture()
{
	if( !channel ) throw("channel not init?");

	//BASS_CHANNELINFO ci;
	//BASS_ChannelGetInfo(channel,&ci); // get number of channels
	float dampening = 0.7f;

	// The idea is to save many previous fft reads so we get an average over time. This
	// may result in a more visually pleasing effect. Trying to smooth out short jitters. Also. It
	// accumulates volume over time. so high peaks stay higher longer.
/*
	for(int i=0; i<1024; i++)
		previous_fft[0][i] = fft[i] * dampening;

	for(int i=1; i<NUM_SMOOTHING_BUFS; i++)
	for(int j=0; j<1024; j++)
	{
		previous_fft[NUM_SMOOTHING_BUFS-i][j] = previous_fft[NUM_SMOOTHING_BUFS-i-1][j] * dampening;
	}
*/
	
	BASS_ChannelGetData(channel,fft,BASS_DATA_FFT2048); // 2048 sample FFT gives 1024 floating-point values
}

bool BassWrapper::init_fft_storage(int num_bands)
{
	return false;
}

void BassWrapper::init_sample_capture( std::vector<float>& buf )
{
		//BASS_CHANNELINFO ci;
		//BASS_ChannelGetInfo(channel,&ci); // get number of channels
		//buf=alloca(ci.chans*SPECWIDTH*sizeof(float)); // allocate buffer for data
		BASS_ChannelGetData(channel,&buf[0],(buf.size()*sizeof(float))|BASS_DATA_FLOAT); // get the sample data (floating-point to avoid 8 & 16 bit processing)
}


void BassWrapper::play(std::string musicFilePath)
{
	HWND win = NULL;

	if (!(channel=BASS_StreamCreateFile(FALSE,"",0,0,BASS_SAMPLE_LOOP))
		&& !(channel=BASS_MusicLoad(FALSE,musicFilePath.c_str(),0,0,BASS_MUSIC_RAMP|BASS_SAMPLE_LOOP,0))) {
			char strBuf[200];
			sprintf_s(strBuf,200,"Can't play file %s", musicFilePath.c_str() );
			throw(strBuf);
	}

	BASS_ChannelPlay(channel,true);
}

void BassWrapper::getLevel( float *left, float *right )
{
	DWORD level;
	level = BASS_ChannelGetLevel(channel);
	(*left) = LOWORD(level) / 32768.0f; // the left level
	(*right) = HIWORD(level) / 32768.0f; // the right level
}

float BassWrapper::get_fft_value( int idx )
{
	if( idx < 0 || idx > 1024 ) throw("BassWrapp: fft out of bounds");
	return fft[idx];
	//return 1;
}

float BassWrapper::get_prev_fft_value( int idx )
{
	if( idx < 0 || idx > 1024 ) throw("BassWrapp: fft out of bounds");

	float summedLevels = 0.0f;
	for(int i=0; i<NUM_SMOOTHING_BUFS; i++)
	{
		summedLevels += previous_fft[i][idx];
	}

	return summedLevels;// / float(NUM_SMOOTHING_BUFS);
	//return 1;
}

void BassWrapper::playSound( char *soundFilePath )
{
	BASS_StreamFree(stream);
	stream=BASS_StreamCreateFile(FALSE, soundFilePath, 0, 0, 0);
	BASS_ChannelPlay(stream,0);
}

unsigned long BassWrapper::getHandleAndLoad( char *soundFilePath )
{
	return BASS_StreamCreateFile(FALSE, soundFilePath, 0, 0, 0);
}

void BassWrapper::playStream( unsigned int handle )
{
	BASS_ChannelPlay(handle,0);
}

void BassWrapper::freeStream( unsigned int handle )
{
	BASS_StreamFree(handle);
}

// Recording callback - not doing anything with the data
BOOL CALLBACK _DuffRecording(HRECORD handle, const void *buffer, DWORD length, void *user)
{
	return 1; // continue recording
}


bool BassWrapper::init_loopback( int recording_device )
{
	// initialize BASS recording
	if (!BASS_RecordInit(recording_device)) {
		fprintf(stderr,"Can't initialize device\n");
		return false;
	}
	
	// 44100 Hz, mono
	if ( !(channel=BASS_RecordStart(44100, 1, BASS_SAMPLE_FLOAT, _DuffRecording, 0)) )
	{
		fprintf(stderr,"Can't start recording\n");
		return false;
	}
	
	is_recording = true;

	return true;
}



void BassWrapper::list_playback_devices()
{
	int a, count=0;
	BASS_DEVICEINFO info;
	for (a=0; BASS_GetDeviceInfo(a, &info); a++){

		if (info.flags&BASS_DEVICE_ENABLED) { // device is enabled
			count++; // count it 
		}
	}
}
void BassWrapper::list_recording_devices()
{
	int a, count=0;
	BASS_DEVICEINFO info;
	for (a=0; BASS_RecordGetDeviceInfo(a, &info); a++){
		if (info.flags&BASS_DEVICE_ENABLED) { // device is enabled

			printf("devno: [%i] is a %s\n", a, info.name);

			count++; // count it 
		}
	}
}






