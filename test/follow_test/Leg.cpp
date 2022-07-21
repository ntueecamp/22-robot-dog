#include "Leg.h"

Leg dogLeg;

Leg::Leg()
 : leftChannel(DEFAULT_LEFT_CHANNEL), rightChannel(DEFAULT_RIGHT_CHANNEL),
   leftPin(DEFAULT_LEFT_PIN), rightPin(DEFAULT_RIGHT_PIN) {

}

Leg::~Leg() {

}

void Leg::initialize() {
  leftServo.initialize(leftChannel, leftPin, LEFT_MIN_US, RIGHT_MAX_US);
  rightServo.initialize(rightChannel, rightPin, RIGHT_MIN_US, RIGHT_MAX_US);
}

void Leg::write(double leftSetPoint, double rightSetPoint) {
  // servos are placed in different orientation, so one of them should be reversed
  leftServo.write(leftSetPoint * (1.0));
  rightServo.write(rightSetPoint * (-1.0));
}
