#include "Leg.h"

void Leg::initialize() {
  leftPin = DEFAULT_LEFT_PIN;
  rightPin = DEFAULT_RIGHT_PIN;
  leftChannel = DEFAULT_LEFT_CHANNEL;
  rightChannel = DEFAULT_RIGHT_CHANNEL;

  leftServo.initialize(leftChannel, leftPin, LEFT_MIN_US, RIGHT_MAX_US);
  rightServo.initialize(rightChannel, rightPin, RIGHT_MIN_US, RIGHT_MAX_US);
}

void Leg::write(double leftSetPoint, double rightSetPoint) {
  // servos are placed in different rotation, so one of them should be reversed
  leftServo.write(leftSetPoint * (1.0));
  rightServo.write(rightSetPoint * (-1.0));
}
