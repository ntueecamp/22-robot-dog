#ifndef NEW_AUDIO_H
#define NEW_AUDIO_H
#include "XT_DAC_Audio.h"
class New_Audio:public XT_Wav_Class
{
	private:
		int audio_size;
		double audio_time;
	public:
		New_Audio(const unsigned char *data, int siz);
		void play_music(int times, int interval);	
};
#endif