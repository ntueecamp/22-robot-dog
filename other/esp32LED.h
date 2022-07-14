#ifndef esp32LED_h
#define esp32LED_h

#include "Arduino.h"
#include <SPI.h>
#include "LedMatrix.h"

#define NUMBER_OF_DEVICES 1 
#define CS_PIN 5
#define CLK_PIN 18
#define MISO_PIN 22 
#define MOSI_PIN 23


class eps32LED {
private:
  LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CLK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  char text[10] = "Robot_dog";
  byte pattern[9] = {B11111111,
                     B11111111,
                     B11111111,
                     B11111111,
                     B11111111,
                     B11111111,
                     B11111111,
                     B11111111};
public:
  eps32LED();

  /*
   * Initialize the LED Matrix
   */
  void initialize();

  /*
   * Set the text to print
   * Input should be 9 letters
   * Should be put in setup(){}
   */
  void set_text(char input_text[10]);

  /*
   * Set the pattern to print
   * Input should be a array containing 8 bytes
   * Can be put in setup(){} or loop(){}
   */
  void set_pattern(byte input_pattern[9]);

  /*
   * Print the text
   * It will delay by itself (just put LED01.print_text() in loop(){} ).
   */
  void print_text();

  /*
   * Print the pattern
   * It will not delay by itself(code may look like: 
   * LED01.set_pattern(pattern01);
   * LED01.print_pattern();
   * delay(1000);
   * LED01.set_pattern(pattern02);
   * LED01.print_pattern();
   * delay(1000)
   * )
   */
  void print_pattern();
};

#endif
