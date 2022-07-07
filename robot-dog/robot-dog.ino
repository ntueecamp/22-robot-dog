/**
 * @file 22-robot-dog.ino
 * @author ntueecamp 2022
 * @brief A robot dog that can interact with people and follow their owner.
 * @date 2022-
 * 
 */

#define DEBUG

#include "Interact.h"
#include "Follow.h"

#define CAP_TOUCH_PIN      4
#define LIMIT_SWITCH_PIN   12
#define PHOTO_RESISTOR_PIN 32

#define SOUND_PIN          25

#define SCK_PIN            18    // CLK on LED
#define MISO_PIN           19    // not used
#define MOSI_PIN           23    // DIN on LED
#define CS_PIN             5     // CS on LED

void setup() {
  // put your setup code here, to run once:
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("****Debug mode****\n");
#endif

  // interact inputs
  initCapTouch(CAP_TOUCH_PIN, 50);
  initLimitSwitch(LIMIT_SWITCH_PIN, RISING);
  initPhotoResistor(PHOTO_RESISTOR_PIN, 1500, 300);    // with 10 kohm to GND

  // interact outputs
  initSound(SOUND_PIN);
  initLED(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  
  // follow
  initFollow();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Dont put anything here, use RTOS task instead
}
