#include "ESP32Servo.h"


void ESP32Servo::initialize(int _channel, int _servoPin, double _minSpeed, double _maxSpeed) {
  ESP32Servo::channel = _channel;
  ESP32Servo::servoPin = _servoPin;

  ESP32Servo::minSpeed = _minSpeed;
  ESP32Servo::maxSpeed = _maxSpeed;

  ledcSetup(ESP32Servo::channel, PWM_FREQ, TIMER_WIDTH);
  ledcAttachPin(ESP32Servo::servoPin, ESP32Servo::channel);
}

void ESP32Servo::write(double R) {
  // set speed range constraint
  R = min(1.0, max(-1.0, R));
  // map speed to pulse width
  int timer_count = fmap(R, ESP32Servo::minSpeed, ESP32Servo::maxSpeed, MIN_DUTY, MAX_DUTY) / PWM_CYCLE * TIMER_RESOLUTION;
  ledcWrite(ESP32Servo::channel, timer_count);
}

double ESP32Servo::fmap(double val, double min1, double max1, double min2, double max2) {
  return (val - min1) / (max1 - min1) * (max2 - min2) + min2;
}
