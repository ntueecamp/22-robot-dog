#include "ESP360Servo.h"


void ESP360Servo::initialize(int _channel, int _servoPin, double _minSpeed, double _maxSpeed) {
  ESP360Servo::channel = _channel;
  ESP360Servo::servoPin = _servoPin;

  ESP360Servo::minSpeed = _minSpeed;
  ESP360Servo::maxSpeed = _maxSpeed;

  ledcSetup(ESP360Servo::channel, PWM_FREQ, TIMER_WIDTH);
  ledcAttachPin(ESP360Servo::servoPin, ESP360Servo::channel);
}

void ESP360Servo::write(double R) {
  // set speed range constraint
  R = min(1.0, max(-1.0, R));
  // map speed to pulse width
  int timer_count = fmap(R, ESP360Servo::minSpeed, ESP360Servo::maxSpeed, MIN_DUTY, MAX_DUTY) / PWM_CYCLE * TIMER_RESOLUTION;
  ledcWrite(ESP360Servo::channel, timer_count);
}

double ESP360Servo::fmap(double val, double min1, double max1, double min2, double max2) {
  return (val - min1) / (max1 - min1) * (max2 - min2) + min2;
}
