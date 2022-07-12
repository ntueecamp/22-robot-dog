#ifndef Leg_h
#define Leg_h

#include "Arduino.h"
#include "ESP32Servo.h"

#define DEFAULT_LEFT_CHANNEL  0
#define DEFAULT_RIGHT_CHANNEL 1
#define DEFAULT_LEFT_PIN      15
#define DEFAULT_RIGHT_PIN     2

#define LEFT_MIN_US           500
#define LEFT_MAX_US           2500
#define RIGHT_MIN_US          500
#define RIGHT_MAX_US          2500


class Leg {

private:

  ESP32Servo leftServo, rightServo;

  int leftChannel, rightChannel;
  int leftPin, rightPin;

public:

  // set channel and pin with specified or default values
  void initialize();

  // set servo speed from -1.0 to 1.0
  void write(double leftSetPoint, double rightSetPoint);
  
};

#endif
