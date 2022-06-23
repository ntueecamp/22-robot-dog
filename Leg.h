#ifndef Leg_h
#define Leg_h

#include "Arduino.h"
#include "ESP32Servo.h"

#define MIN_SPEED             0.0
#define MAX_SPEED             180.0

#define DEFAULT_LEFT_CHANNEL  0
#define DEFAULT_RIGHT_CHANNEL 1
#define DEFAULT_LEFT_PIN      18
#define DEFAULT_RIGHT_PIN     19


class Leg {

private:

  ESP32Servo leftServo, rightServo;

  int leftChannel, rightChannel;
  int leftPin, rightPin;

  void initializeServo();

public:

  // set channel and pin with specified or default values
  void initialize();
  void initialize(int _leftPin, int _rightPin);
  void initialize(int _leftChannel, int _rightChannel, int _leftPin, int _rightPin);

  // set servo speed
  void write(double leftR, double rightR);
  // calculate speed by mapping before setting speed
  double mapWrite(double leftVal, double rightVal, double minVal, double maxVal);
  
};

#endif
