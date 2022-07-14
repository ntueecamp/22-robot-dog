#include "XT_DAC_Audio.h"
#include "new_audio.h"
#include "woof.h"
#include "low_woof.h"
#include "playback1.h"
#include "sound_A.h"

New_Audio Sound_High(low_woof,size_low_woof);
New_Audio Sound_Low(playback1,size_playback1);        
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
    if(dat=="high")
    {
      Sound_High.play_once();
    }
    else if(dat=="low")
    {
      Sound_Low.play_once();
    }
    else if(dat=="sin")
    {
      Sound_Sin.play_once();
    }
  }
}
