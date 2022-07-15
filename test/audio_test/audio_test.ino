#include "XT_DAC_Audio.h"
#include "new_audio.h"
#include "woof.h"
#include "low_woof.h"
#include "playback1.h"
#include "sound_A.h"

New_Audio Sound_1(woof,size_woof);
New_Audio Sound_2(low_woof,size_low_woof);
New_Audio Sound_3(playback1,size_playback1);        
New_Audio Sound_Sin(sound_A,size_sound_A);  
                                      
XT_DAC_Audio_Class DacAudio(25,0);              

void setup() {
  Serial.begin(115200);          
}

void loop() {
  if(Serial.available()>0)
  {
    String dat=Serial.readStringUntil('\n');
    Serial.println(dat);
    if(dat=="start")
    {
      for(int i=0;i<10;i++)
      {
        Sound_1.play_once();
        Sound_2.play_once();
        Sound_3.play_once();
      }
    }
  }
}
