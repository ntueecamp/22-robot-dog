#include "Trace.h"

Trace::Trace() {
}

Trace::~Trace() {

}

void Trace::init() {
    radar.init();
    leg.initialize();
}

void Trace::Move() {

    // base velocity
    // delta velocity (velocity difference between the left/right wheels)
    // minimum distance from the owner to the dog
    // direction presented by binary number
    double baseVel, deltaVel, leftVel = 0, rightVel = 0, dis;
    int dir;

    // make the radar to rotation for a period and the ultra-sonic to detect
    radar.radarRotation();

    // get min dis from the dog
    dis = radar.getDis();

    // compute base velocity according to the min distance
    // to make the base velocity be negative to move backward when the owner is too close 
    // to make the base velocity be smaller then 1.0
    baseVel = (dis - THRESHOLD/4)/THRESHOLD;

    // make base velocity be half when the dog would move forward
    // make base velocity be twice when the dog would move backward
    // the operation is to make the dog move more smoothly
    if(baseVel > 0)
      baseVel /= 2;
    else baseVel *= 2;

    // get the direction of the owner
    dir = radar.getDir();

    // if the dog fails to detect its owner , the dog will stop 
    // (base velocity equals to zero, and delta velocity equals to zero)
    // and wait for the next detection
    if(dir == 0) {
      baseVel = 0;
    }

    // compute for its delta velocity by p control according to the direction
    deltaVel = ((dir&16) >> 4 )* LL + ((dir&8) >> 3) * L + ((dir&4) >> 2) * C + ((dir&2) >> 1) * R + (dir&1) * RR;
    
    // 1. avoid one of the wheel not reverse rotation
    // 2. to make the dog to at least move forward when turning
    //    both side of the wheel need to have a lower bound velocity in absolute value
    //    the lower bound will be +/- 0.22
    if(baseVel > 0){
      if(baseVel + deltaVel > 0) leftVel = baseVel + deltaVel;
      if(baseVel - deltaVel > 0) rightVel = baseVel - deltaVel;
      leftVel = max(leftVel, 0.22);
      rightVel = max(rightVel, 0.22);
    }else if(baseVel < 0){
      if(baseVel + deltaVel < 0) leftVel = baseVel + deltaVel;
      if(baseVel - deltaVel < 0) rightVel = baseVel - deltaVel;
      leftVel = min(leftVel, -0.22);
      rightVel = min(rightVel, -0.22);
    }
    leg.write(leftVel, rightVel);
}
