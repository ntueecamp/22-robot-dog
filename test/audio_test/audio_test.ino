#include "XT_DAC_Audio.h"
#include "new_audio.h"
#include "woof.h"
#include "low_woof.h"

New_Audio Sound_High(woof,size_woof);
New_Audio Sound_Low(low_woof,size_low_woof);          
                                      
XT_DAC_Audio_Class DacAudio(25,0);              

void setup() {
  Serial.begin(115200);          
}

void loop() {
  if(Serial.available()>0)
  {
    String dat=Serial.readStringUntil('\n');
    if(dat=="high")
    {
      Sound_High.play_music(5,300);
    }
    else if(dat=="low")
    {
      Sound_Low.play_music(5,300);
    }
  }
}
