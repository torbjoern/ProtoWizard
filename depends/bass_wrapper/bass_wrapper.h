#include <vector>

class BassWrapper
{
public:
	BassWrapper();
	~BassWrapper();
	void play(std::string musicFilePath);
	void playSound(char *soundFilePath);
	void playStream( unsigned int handle );
	void freeStream( unsigned int handle );
	unsigned long getHandleAndLoad( char *soundFilePath );
	void update();
	void getLevel( float *left, float *right );
	float get_fft_value(int idx);
	float get_prev_fft_value( int idx );

	bool init_loopback( int recording_device );


	void list_playback_devices();
	void list_recording_devices();

private:

	typedef unsigned long       DWORD;
	DWORD channel; // Used by BASS-sound library
	float fft[1024];
	static const int NUM_SMOOTHING_BUFS = 60;
	float previous_fft[NUM_SMOOTHING_BUFS][1024];
	int SPECWIDTH;

	unsigned long stream;

	bool is_recording;



};