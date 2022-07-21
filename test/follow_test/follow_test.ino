/**
 * @file 22-robot-dog.ino
 * @author ntueecamp 2022
 * @brief A robot dog that can interact with people and follow its owner.
 * @date 2022-
 * 
 */

#include "Trace.h"

Trace dog;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  dogLeg.initialize();
  dog.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  dog.Move();
}
