#ifndef ESP32Servo_h
#define ESP32Servo_h

#include "Arduino.h"

#define DEFAULT_MIN_US           979.0
#define DEFAULT_MAX_US           1979.0

#define MIN_SETPOINT             -1.0
#define MAX_SETPOINT             1.0

#define PWM_FREQ                 50
#define PWM_CYCLE                (1000000.0 / PWM_FREQ)
#define TIMER_WIDTH              16
#define TIMER_RESOLUTION         ((1 << TIMER_WIDTH) - 1)


class ESP32Servo {

private:

  int channel;
  int servoPin;

  double minUs;
  double maxUs;

  // re-define map function with double type
  double _fmap(double val, double min1, double max1, double min2, double max2);

  void _initialize(int _channel, int _servoPin, double _minUs, double _maxUs);

public:

  // specify channel and pin of led_c, and the range of servo speed
  void initialize(int _channel, int _servoPin);
  void initialize(int _channel, int _servoPin, double _minUs, double _maxUs);

  // set servo speed range from -1 to 1
  void write(double setPoint);

};

#endif
