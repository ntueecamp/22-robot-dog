#ifndef _RADAR_H_
#define _RADAR_H_

#include "Servo.h"

// constants
#define TRIGPIN 27
#define ECHOPIN 14
#define RADARSERVOPIN 13

#define ULTRASONIC_TIMEOUT 150  // ms
#define RADARDELAYTIME 120      // ms

#define CENTERANGLE 90   // degrees
#define ROTATEANGLE 30  // degrees

#define THRESHOLD 100  // cm

class Radar {
   public:
    // constructor & destructor
    Radar();
    ~Radar();

    // initialize
    void init();

    // belows are the output of the results of detection of ultra-sonic

    // get mean distance from robot dog to its owner
    // value from 0 to threshold
    int getDis();

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
    Servo servo;

    // distance array
    int curDis[5];
    int preDis[5];

    // ultra-sonic measuring
    int disMeasuring();

    // data calculate
    void calculate();

    // mean distance from robot dog to its owner
    float meanDis;
    // use a binary number to presents sth in the direction
    short direction;

    // current angle of radar
    int angle;

    // delta angle of radar
    int deltaAngle;
};

#endif /* _RADAR_H_ */