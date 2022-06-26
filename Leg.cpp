#include "Leg.h"


void Leg::initialize() {
  Leg::leftPin = DEFAULT_LEFT_PIN;
  Leg::rightPin = DEFAULT_RIGHT_PIN;
  Leg::leftChannel = DEFAULT_LEFT_CHANNEL;
  Leg::rightChannel = DEFAULT_RIGHT_CHANNEL;

  Leg::initializeServo();
}

void Leg::initialize(int _leftPin, int _rightPin) {
  Leg::leftPin = _leftPin;
  Leg::rightPin = _rightPin;
  Leg::leftChannel = DEFAULT_LEFT_CHANNEL;
  Leg::rightChannel = DEFAULT_RIGHT_CHANNEL;

  Leg::initializeServo();
}

void Leg::initialize(int _leftChannel, int _rightChannel, int _leftPin, int _rightPin) {
  Leg::leftPin = _leftPin;
  Leg::rightPin = _rightPin;
  Leg::leftChannel = _leftChannel;
  Leg::rightChannel = _rightChannel;

  Leg::initializeServo();
}

void Leg::initializeServo() {
  Serial.printf("%d, %d, %f, %f\n", leftChannel, leftPin, MIN_SPEED, MAX_SPEED);
  Leg::leftServo.initialize(leftChannel, leftPin, MIN_SPEED, MAX_SPEED);
  Leg::rightServo.initialize(rightChannel, rightPin, MIN_SPEED, MAX_SPEED);
}

void Leg::write(double leftR, double rightR) {
  Leg::leftServo.write(leftR);
  Leg::rightServo.write(rightR);
}

double Leg::mapWrite(double leftVal, double rightVal, double minVal, double maxVal) {
  Leg::leftServo.mapWrite(leftVal, minVal, maxVal);
  Leg::rightServo.mapWrite(rightVal, minVal, maxVal);
}
