#include "ESP32Servo.h"


void ESP32Servo::initialize(int _channel, int _servoPin) {
  _initialize(_channel, _servoPin, DEFAULT_MIN_US, DEFAULT_MAX_US);
}

void ESP32Servo::initialize(int _channel, int _servoPin, double _minUs, double _maxUs) {
  _initialize(_channel, _servoPin, _minUs, _maxUs);
}

void ESP32Servo::_initialize(int _channel, int _servoPin, double _minUs, double _maxUs) {
  minUs = _minUs;
  maxUs = _maxUs;

  channel = _channel;
  servoPin = _servoPin;

  ledcSetup(channel, PWM_FREQ, TIMER_WIDTH);
  ledcAttachPin(servoPin, channel);
}

void ESP32Servo::write(double R) {
  // set speed range constraint
  R = min(MAX_SETPOINT, max(MIN_SETPOINT, R));
  // map speed to pulse width
  int timer_count = _fmap(R, MIN_SETPOINT, MAX_SETPOINT, minUs, maxUs) / PWM_CYCLE * TIMER_RESOLUTION;
  ledcWrite(channel, timer_count);
}

double ESP32Servo::_fmap(double val, double min1, double max1, double min2, double max2) {
  return (val - min1) / (max1 - min1) * (max2 - min2) + min2;
}

