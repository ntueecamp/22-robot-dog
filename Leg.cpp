#include "Leg.h"


void Leg::initialize() {
  Leg::leftPin = DEFAULT_LEFT_PIN;
  Leg::rightPin = DEFAULT_RIGHT_PIN;
  Leg::leftChannel = DEFAULT_LEFT_CHANNEL;
  Leg::rightChannel = DEFAULT_RIGHT_CHANNEL;

  Serial.printf("%d, %d, %f, %f\n", leftChannel, leftPin, MIN_SPEED, MAX_SPEED);
  Leg::leftServo.initialize(leftChannel, leftPin, MIN_SPEED, MAX_SPEED);
  Leg::rightServo.initialize(rightChannel, rightPin, MIN_SPEED, MAX_SPEED);
}

void Leg::write(double leftR, double rightR) {
  // servos are placed in different rotation, so one of them should be reversed
  Leg::leftServo.write(leftR * (1.0));
  Leg::rightServo.write(rightR * (-1.0));
}
