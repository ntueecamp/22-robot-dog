/**
 * @file 22-robot-dog.ino
 * @author ntueecamp 2022
 * @brief A robot dog that can interact with people and follow their owner.
 * @date 2022-
 * 
 */

#define DEBUG

#include "Interact_Inputs.h"
#include "Leg.h"

#define Cap_Touch_Pin      4
#define Limit_Switch_Pin   12
#define Photo_Resistor_Pin 32

void setup() {
  // put your setup code here, to run once:
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("****Debug mode****\n");
#endif

  initCapTouch(Cap_Touch_Pin, 500);   // TBD
  initLimitSwitch(Limit_Switch_Pin, RISING);
  initPhotoResistor(Photo_Resistor_Pin, 1500, 1000);  // TBD
}

void loop() {
  // put your main code here, to run repeatedly:
  // Dont put anything here, use RTOS task instead
}
