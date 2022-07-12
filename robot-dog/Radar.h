#ifndef _RADAR_H_
#define _RADAR_H_

#include "ESP32Servo.h"

// constants
#define TRIGPIN 27
#define ECHOPIN 14
#define RADARSERVOPIN 13

#define ULTRASONIC_TIMEOUT 30  // ms
#define RADARDELAYTIME 120     // ms

#define CENTERANGLE 0
#define LEFT_HALF_ROTATEANGLE 0.26
#define RIGHT_HALF_ROTATEANGLE 0.31

#define DEFAULT_NECK_CHANNEL 2
#define NECKSERVO_MIN_US 480
#define NECKSERVO_MAX_US 2380

// THRESHOLD is the distance for dog to decide moving forward or backward
// _THRESHOLD is the effective distance which is to determine if owner is there
#define THRESHOLD 25    // cm
#define _THRESHOLD 4 * THRESHOLD  // cm

class Radar {
   public:
    // constructor & destructor
    Radar();
    ~Radar(){};

    // initialize
    void init();

    // belows are the output of the results of detection of ultra-sonic

    // get mean distance from robot dog to its owner
    // value from 0 to threshold
    double getDis();

    // get results of sensing if there're sth in each direction
    // using every bit of the binary number to show if there're sth
    // 5 bits for 5 direction
    // returning value ranging from 0 to 31
    // for example, 11000 in binary, 24 in decimal, means that there're sth situated in the left-most direction and the left direction
    short getDir();

    // radar rotation
    // rotation by theta(variable of this class) each time
    // and rotation one rounds at each calling
    void radarRotation();

   private:
    // servo of radar
    ESP32Servo servo;

    // distance array
    double Dis[5];

    // ultra-sonic measuring
    double disMeasuring();

    // data calculate
    void calculate();

    // mean distance from robot dog to its owner
    double meanDis;
    // use a binary number to presents sth in the direction
    short direction;

    // current angle of radar
    float angle;

    // delta angle of radar
    float deltaAngle;
    double minDis;
};

#endif /* _RADAR_H_ */
