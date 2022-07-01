#include "ESP180Servo.h"


void ESP180Servo::initialize(int _channel, int _servoPin) {
  ESP180Servo::channel = _channel;
  ESP180Servo::servoPin = _servoPin;

  ledcSetup(ESP180Servo::channel, PWM_FREQ, TIMER_WIDTH);
  ledcAttachPin(ESP180Servo::servoPin, ESP180Servo::channel);
}

void ESP180Servo::write(double R) {
  // set speed range constraint
  R = min(ESP180Servo::MAX_POS, max(ESP180Servo::MIN_POS, R));
  // map speed to pulse width
  int timer_count = fmap(R, ESP180Servo::MIN_POS, ESP180Servo::MAX_POS, MIN_DUTY, MAX_DUTY) / PWM_CYCLE * TIMER_RESOLUTION;
  ledcWrite(ESP180Servo::channel, timer_count);
}

double ESP180Servo::fmap(double val, double min1, double max1, double min2, double max2) {
  return (val - min1) / (max1 - min1) * (max2 - min2) + min2;
}
