#include "Leg.h"


void Leg::_initialize() {
  leftPin = DEFAULT_LEFT_PIN;
  rightPin = DEFAULT_RIGHT_PIN;
  leftChannel = DEFAULT_LEFT_CHANNEL;
  rightChannel = DEFAULT_RIGHT_CHANNEL;
}

void Leg::initialize() {
  _initialize();
  leftServo.initialize(leftChannel, leftPin);
  rightServo.initialize(rightChannel, rightPin);
}

void Leg::initialize(double _leftMinUs, double _leftMaxUs, double _rightMinUs, double _rightMaxUs) {
  _initialize();
  leftServo.initialize(leftChannel, leftPin, _leftMinUs, _leftMaxUs);
  rightServo.initialize(rightChannel, rightPin, _rightMinUs, _rightMaxUs);
}

void Leg::write(double leftR, double rightR) {
  // servos are placed in different rotation, so one of them should be reversed
  leftServo.write(leftR * (1.0));
  rightServo.write(rightR * (-1.0));
}
