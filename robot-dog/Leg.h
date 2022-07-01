#ifndef Leg_h
#define Leg_h

#include "Arduino.h"
#include "ESP360Servo.h"

#define MIN_SPEED             -1.0
#define MAX_SPEED             1.0

#define DEFAULT_LEFT_CHANNEL  0
#define DEFAULT_RIGHT_CHANNEL 1
#define DEFAULT_LEFT_PIN      18
#define DEFAULT_RIGHT_PIN     19


class Leg {

private:

  ESP360Servo leftServo, rightServo;

  int leftChannel, rightChannel;
  int leftPin, rightPin;

public:

  // set channel and pin with specified or default values
  void initialize();

  // set servo speed from -1.0 to 1.0
  void write(double leftR, double rightR);
  
};

#endif
