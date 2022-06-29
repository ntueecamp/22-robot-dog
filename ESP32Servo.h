#ifndef ESP32Servo_h
#define ESP32Servo_h

#include "Arduino.h"

#define MID_DUTY         1479
#define HALF_DUTY_WIDTH  500
#define MIN_DUTY         (MID_DUTY - HALF_DUTY_WIDTH)
#define MAX_DUTY         (MID_DUTY + HALF_DUTY_WIDTH)

#define PWM_FREQ         50
#define PWM_CYCLE        (1000000.0 / PWM_FREQ)
#define TIMER_WIDTH      16
#define TIMER_RESOLUTION ((1 << TIMER_WIDTH) - 1)


class ESP32Servo {

private:

  int channel;
  int servoPin;

  double minSpeed;
  double maxSpeed;

  // re-define map function with double type
  double fmap(double val, double min1, double max1, double min2, double max2);

public:

  // specify channel and pin of led_c, and the range of servo speed
  void initialize(int _channel, int _servoPin, double _minSpeed, double _maxSpeed);

  // set servo speed
  // range from -1 to 1
  void write(double R);

};

#endif
