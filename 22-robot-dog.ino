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

#define CAP_TOUCH_PIN      4
#define LIMIT_SWITCH_PIN   12
#define PHOTO_RESISTOR_PIN 32

void setup() {
  // put your setup code here, to run once:
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("****Debug mode****\n");
#endif

  initCapTouch(CAP_TOUCH_PIN, 50);
  initLimitSwitch(LIMIT_SWITCH_PIN, RISING);
  initPhotoResistor(PHOTO_RESISTOR_PIN, 1500, 300);    // with 10 kohm to GND
}

void loop() {
  // put your main code here, to run repeatedly:
  // Dont put anything here, use RTOS task instead
}
