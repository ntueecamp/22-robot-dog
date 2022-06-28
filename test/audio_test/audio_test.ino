#include "MusicDefinitions.h"
#include "XT_DAC_Audio.h"
#include "new_audio.h"
#include <time.h>
#include "woof.h"

New_Audio Sound(woof,size_woof);     
                                      
XT_DAC_Audio_Class DacAudio(25,0);              

void setup() {
  Serial.begin(115200);          
}

void loop() {
  Sound.play_music(5,500);
  delay(5000);
}
