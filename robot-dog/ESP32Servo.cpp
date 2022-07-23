#include "ESP32Servo.h"

#include <Arduino.h>

ESP32Servo::ESP32Servo() {

}

ESP32Servo::~ESP32Servo() {

}

void ESP32Servo::initialize(int _channel, int _servoPin, double _minUs, double _maxUs) {
  minUs = _minUs;
  maxUs = _maxUs;

  channel = _channel;
  servoPin = _servoPin;

  ledcSetup(channel, PWM_FREQ, TIMER_WIDTH);
  ledcAttachPin(servoPin, channel);
}

void ESP32Servo::write(double setPoint) {
  // set speed range constraint
  setPoint = min(MAX_SETPOINT, max(MIN_SETPOINT, setPoint));
  // map speed to pulse width
  int timer_count = _fmap(setPoint, MIN_SETPOINT, MAX_SETPOINT, minUs, maxUs) / PWM_CYCLE * TIMER_RESOLUTION;
  ledcWrite(channel, timer_count);
}

double ESP32Servo::_fmap(double val, double min1, double max1, double min2, double max2) {
  return (val - min1) / (max1 - min1) * (max2 - min2) + min2;
}
