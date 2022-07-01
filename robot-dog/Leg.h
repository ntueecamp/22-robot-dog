#ifndef Leg_h
#define Leg_h

#include "Arduino.h"
#include "ESP32Servo.h"

#define DEFAULT_LEFT_CHANNEL  0
#define DEFAULT_RIGHT_CHANNEL 1
#define DEFAULT_LEFT_PIN      18
#define DEFAULT_RIGHT_PIN     19


class Leg {

private:

  ESP32Servo leftServo, rightServo;

  int leftChannel, rightChannel;
  int leftPin, rightPin;

  void _initialize();

public:

  // set channel and pin with specified or default values
  void initialize();
  void initialize(double _leftMinUs, double _leftMaxUs, double _rightMinUs, double _rightMaxUs);

  // set servo speed from -1.0 to 1.0
  void write(double leftR, double rightR);
  
};

#endif
