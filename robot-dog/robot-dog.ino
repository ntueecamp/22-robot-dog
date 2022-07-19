/**
 * @file 22-robot-dog.ino
 * @author ntueecamp 2022
 * @brief A robot dog that can interact with people and follow its owner.
 * @date 2022-
 * 
 */

#define DEBUG

#include "driver/adc.h"
#include "Interact.h"
#include "Trace.h"
#include "AudioRecognition.h"
#include "woof.h"

#define CAP_TOUCH_PIN      4
#define LIMIT_SWITCH_PIN   12
#define PHOTO_RESISTOR_PIN 32

#define SOUND_PIN          25

#define MIC_PIN            34
#define ADC_CHANNEL        ADC1_CHANNEL_6   // GPIO 34

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
  initLimitSwitch(LIMIT_SWITCH_PIN, FALLING);
  initPhotoResistor(PHOTO_RESISTOR_PIN, 1500, 300);    // with 10 kohm to GND

  // interact outputs
  initSound(SOUND_PIN, woof);
  initLED(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  // follow
  dogLeg.initialize();
  initFollow();

  // microphone
  initAudioRecognition(MIC_PIN, ADC_CHANNEL);

#ifdef DEBUG
  Serial.println("Setup Done!");
#endif
}

#ifdef DEBUG
  #include "Events.h"
  EventBits_t dogBits;
#endif

void loop() {
  // put your main code here, to run repeatedly:
  // Dont put anything here, use RTOS task instead
#ifdef DEBUG
  dogBits = xEventGroupGetBits(dogEventGroup);
  Serial.print((dogBits & CAP_TOUCH_BIT)      > 0);
  Serial.print((dogBits & LIMIT_SWITCH_BIT)   > 0);
  Serial.print((dogBits & PHOTO_RESISTOR_BIT) > 0);
  Serial.print((dogBits & FOLLOWING_BIT)      > 0);
  Serial.print((dogBits & FOLLOW_STOP_BIT)    > 0);
  Serial.println("");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
}
