#include "new_audio.h"
#include "soc/rtc_io_reg.h" // https://github.com/WeekendWarrior1/XTronical_XT_DAC_Audio_Mirror/issues/1#issuecomment-1152424853
#include "esp32-hal-timer.h"
#include "XT_DAC_Audio.h"
#include "HardwareSerial.h"
#include "soc/sens_reg.h"  // For dacWrite() patch, TEB Sep-16-2019
#include <time.h>
#define DACPIN 25
XT_DAC_Audio_Class DacAudio(DACPIN,0);

New_Audio::New_Audio(const unsigned char *data, int siz):XT_Wav_Class(data)
{
	audio_size=siz;
	audio_time=siz/8000.0;
  pinMode(DACPIN, OUTPUT);
}
void New_Audio::play_music(int times, int interval)
{
  clock_t start,finish;
	bool in_time=false;
	for(int i=0;i<times;i++)
	{
		while(true)
		{
			DacAudio.FillBuffer();    
			if(!Playing && !in_time)       
			{
				DacAudio.Play(this);
				start=clock();       
				in_time=true;     
			}
			finish=clock();
			if((finish-start+0.0)/(CLOCKS_PER_SEC+0.0)>=0.9*audio_time)
			{
				Serial.println((finish-start+0.0)/(CLOCKS_PER_SEC+0.0));
				DacAudio.StopAllSounds();
				delay(interval);
				in_time=false;
        break;
			}
		}
	}
  digitalWrite(DACPIN, LOW);
  return;
}
