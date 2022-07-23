#ifndef ESP32Servo_h
#define ESP32Servo_h


#define DEFAULT_MIN_US     500
#define DEFAULT_MAX_US     2500

#define MIN_SETPOINT       -1.0
#define MAX_SETPOINT       1.0

#define PWM_FREQ           50
#define PWM_CYCLE          (1000000.0 / PWM_FREQ)
#define TIMER_WIDTH        16
#define TIMER_RESOLUTION   ((1 << TIMER_WIDTH) - 1)

class ESP32Servo {

private:

  int channel;
  int servoPin;

  double minUs;
  double maxUs;

  // re-define map function with double type
  double _fmap(double val, double min1, double max1, double min2, double max2);

public:

  ESP32Servo();
  ~ESP32Servo();

  // specify channel and pin of led_c, and the range of servo speed
  void initialize(int _channel, int _servoPin, double _minUs = DEFAULT_MIN_US, double _maxUs = DEFAULT_MAX_US);

  // set servo speed range from -1 to 1
  void write(double setPoint);

};

#endif
